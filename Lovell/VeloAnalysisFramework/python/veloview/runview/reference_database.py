"""Query and modify the VELO reference run database.

VELO reference database
=======================

The reference database defines what run file is the "reference file" for a
given run and plot.

When looking at plots from a particular run, it is useful to compare each plot
to a reference plot which is known to be "good" by some measure.
However, as the conditions of the dectector change over time the defintion
of "good" changes, and so the reference file for one set of runs may be
different from another. Similarly, it may be that a particular plot within a
set of runs should be compared with a different reference than the rest of the
run.

Ranges of validity
------------------

The reference database (DB) defines ranges of validity (RoV). One reference
file is assigned to each RoV per magnet polarity. Within an RoV, the reference
file for a particular plot may be overriden for all runs within the RoV.

The ranges of validity are defined like a binning schema, and the set of
available run numbers is the set of non-negative integers, so the binning
is defined by a set of boundary runs. Given a single boundary run at e.g. 123,
there are two defined ranges of validity [1]: [0, 123[ and [123, \infty[.

[1]: Sets are given using ISO 31-11 set notation.

Reference runs
--------------

A reference file is uniquely identified by a run number. With this in mind,
the reference database can be thought of as a mapping from 'nominal' plots to
reference plots, and as the set of plots within each run file are the same the
mapping is from nominal plots to reference run numbers. If a given plot in a
given run does not have a reference run number associated with, the reference
run number for the range of validity the run falls in to is used.

Each plot and each range of validity in the database has a corresponding
reference run ID, which can be empty, corresponding to there being no reference
run.

For example, take the following mapping.

    10 -> MagUp 8, MagDown 9 BOUNDARY
    14 -> MagUp 16, MagDown 18 BOUNDARY
    18
     plot_name_A -> 11
    21 -> MagUp 22, MagDown 25 BOUNDARY

There are 3 runs in the database, 10, 14, and 21, and one plot, plot_name_A.
The three runs are all marked as boundaries, and all have magnet up and magnet
down reference runs defined on them. plot_name_A, of run 18, also has a
reference run defined. There are then 4 ranges of validity:
    [0, 10[, [14, 18[, [18, 21[, [21, \infty[
As an example, every magnet up run in [14, 21[ will have run 16 as a reference,
and every magnet down run in the same range will have run 18 as a reference.
The only exception in this case is plot_name_A in run 18, which will have
plot_name_A from run 11 as its reference.
"""
import sqlite3


class InvalidBoundary(Exception):
    pass


class InvalidPlot(Exception):
    pass


class ReferenceDatabase(object):
    """Object responsible for executing CRUD operations on an SQLite3 DB.

    Using a proper ORM with SQLite support here, like SQLAlchemey, here would
    be ideal, but installing third party Python packages in the LHCb software
    environment is a pain, and as we only deal with two resources here, we
    just use SQL statements.
    """
    def __init__(self, path):
        """Initialise a ReferenceDatabase at path.

        The database is loaded if present, else a new one is created using
        the schema at veloview.runview.reference_database.TABLE_SCHEMAS.
        """
        self._load_or_create(path)

    def _load_or_create(self, path):
        """Return Connection object to the DB, creating a new DB if required.

        The list of commands in PRECOMMANDS is executed each time the DB is
        loaded, and the schemas in TABLE_SCHEMAS are loaded if the respective
        table is not present in the DB.
        """
        self.conn = sqlite3.connect(path)
        cur = self.conn.cursor()

        # Run each precommand
        for cmd in PRECOMMANDS:
            cur.execute(cmd)

        # Ensure each table exists
        for table, schema in TABLE_SCHEMAS.iteritems():
            if not self._table_exists(table):
                cur.execute(schema)

        # Make sure anything run is committed to disk
        self.conn.commit()

        return self.conn

    def close(self):
        """Close the database connection by calling sqlite.Connection.close."""
        self.conn.commit()
        self.conn.close()

    def __enter__(self):
        """Enable use of `with` syntax for automatic lifetime management."""
        return self

    def __exit__(self, type, value, trackback):
        self.close()

    def _table_exists(self, table):
        """Return True if the table exists in the database, else False."""
        res = self.conn.execute((
            'SELECT name FROM sqlite_master '
            "WHERE name=? AND type='table' "
            'LIMIT 1'
        ), (table,))
        return res.fetchone() is not None

    def run_boundaries(self):
        """Return list of run boundaries as integers."""
        res = self.conn.execute((
            'SELECT run_id FROM run_boundaries'
        ))
        # fetchall returns a list of tuples of column values for each row, so
        # flatten the tuple as we only want one value
        return [x[0] for x in res.fetchall()]

    def add_run_boundary(self, boundary, up_run, down_run):
        """Add run boundary with reference runs for up and down polarities.

        If the boundary already exists, that is has an identical run number,
        a InvalidBoundary exception is thrown.
        """
        msg = None
        try:
            self.conn.execute((
                'INSERT INTO run_boundaries('
                'run_id, magnet_up_run_id, magnet_down_run_id'
                ') VALUES (?, ?, ?)'
            ), (boundary, up_run, down_run))
        except sqlite3.IntegrityError as e:
            err = e.message
            if err.startswith('UNIQUE constraint failed: '):
                msg = 'Could not add boundary {0}, already exists'.format(
                    boundary
                )
                raise InvalidBoundary(msg)
            elif err.startswith(('datatype mismatch', 'CHECK constraint')):
                msg = 'Could not add boundary {0}, not integer type'.format(
                    boundary
                )
                raise InvalidBoundary(msg)
            else:
                # Re-raise the original exception as we can't handle it
                raise

    def remove_run_boundary(self, boundary):
        """Remove run boundary."""
        self.conn.execute((
            'DELETE FROM run_boundaries '
            'WHERE run_id = ?'
        ), (boundary,))

    def update_run_boundary(self, boundary, up_run=None, down_run=None):
        """Edit run boundary with reference runs for up and down polarities."""
        # Decide what columns to update
        updates = []
        bindings = []
        if up_run:
            updates.append('magnet_up_run_id = ?')
            bindings.append(up_run)
        if down_run:
            updates.append('magnet_down_run_id = ?')
            bindings.append(down_run)
        bindings.append(boundary)

        # Only run the query if there's something to do
        if updates:
            self.conn.execute((
                'UPDATE run_boundaries SET {0} WHERE run_id = ?'
            ).format(', '.join(updates)), bindings)

    def surrounding_boundaries(self, run):
        """Return 2-tuple of run boundaries surrounding run.

        For a run smaller than the lowest run boundary, (0, lowest run) is
        returned. For a run larger than the highest run boundary,
        (0, float('inf')) is returned, where float('inf') is as defined in
        IEEE 754. See http://stackoverflow.com/questions/24587994.
        """
        lower = self.conn.execute((
            'SELECT run_id FROM run_boundaries WHERE run_id <= ? '
            'ORDER BY run_id DESC LIMIT 1'
        ), (run,)).fetchone()
        upper = self.conn.execute((
            'SELECT run_id FROM run_boundaries WHERE run_id > ? '
            'ORDER BY run_id ASC LIMIT 1'
        ), (run,)).fetchone()
        lower = lower[0] if lower else 0
        upper = upper[0] if upper else float('inf')
        return [lower, upper]

    def reference_run(self, run, up=False, down=False):
        """Return reference run for the given run.

        The reference is resolved by finding the closest boundary below or
        equal to run, and inspecting the relevant polarity run number.
        If the run is below the lowest boundary in the database, zero is
        returned.
        What polarity is used depends on what keyword argument is True. If both
        are set as True, or both are set to False, a ValueError is raised.
        """
        if (up and down) or not (up or down):
            raise ValueError('Must specify a single magnet polarity')

        references = self.conn.execute((
            'SELECT magnet_up_run_id, magnet_down_run_id '
            'FROM run_boundaries WHERE run_id <= ? '
            'ORDER BY run_id DESC LIMIT 1'
        ), (run,)).fetchone()
        if not references:
            return 0
        return references[0] if up else references[1]

    def boundary_plots(self, boundary):
        """Return a list of all plots defined on the boundary.

        List of 3-tuples as (plot name, up run number, down run number).
        """
        return self.conn.execute((
            'SELECT plot_name, magnet_up_run_id, magnet_down_run_id '
            'FROM boundary_plots WHERE boundary_run_id = ?'
        ), (boundary,)).fetchall()

    def add_boundary_plot(self, boundary, plot, up_run, down_run):
        msg = None
        try:
            self.conn.execute((
                'INSERT INTO boundary_plots('
                'plot_name, boundary_run_id, magnet_up_run_id, '
                'magnet_down_run_id'
                ') VALUES (?, ?, ?, ?)'
            ), (plot, boundary, up_run, down_run))
        except sqlite3.IntegrityError as e:
            err = e.message
            if err.startswith('FOREIGN KEY constraint failed'):
                msg = 'Boundary {0} does not exist'.format(boundary)
                raise InvalidPlot(msg)
            elif err.startswith('UNIQUE constraint failed'):
                msg = 'Plot name `{0}` already exists at boundary {1}'.format(
                    plot, boundary
                )
                raise InvalidPlot(msg)
            else:
                raise

    def remove_boundary_plot(self, run, plot):
        """Remove boundary plot."""
        self.conn.execute((
            'DELETE FROM boundary_plots '
            'WHERE boundary_run_id = ? AND plot_name = ? '
        ), (run, plot))

    def update_boundary_plot(self, run, plot, up_run=None, down_run=None):
        updates = []
        bindings = []
        if up_run:
            updates.append('magnet_up_run_id = ?')
            bindings.append(up_run)
        if down_run:
            updates.append('magnet_down_run_id = ?')
            bindings.append(down_run)
        bindings += [run, plot]

        # Only run the query if there's something to do
        if updates:
            self.conn.execute((
                'UPDATE boundary_plots SET {0} '
                'WHERE boundary_run_id = ? AND plot_name = ? '
            ).format(', '.join(updates)), bindings)

    def reference_run_for_plot(self, run, plot, up=False, down=False):
        """Find the reference run for the given run and plot.

        The reference run is resolved by trying to find if a reference run has
        been specified for the plot at the relevant run boundary.  As plots are
        overriden per run range, the bin defined by two consecutive, run
        boundaries, the run boundary for the run is found first, then it is
        checked if any plots are specified at this boundary.
        If no plot is specified, the reference run at the boundary is returned.
        See reference_run for how this is done.
        """
        if (up and down) or not (up or down):
            raise ValueError('Must specify a single magnet polarity')

        boundaries = self.surrounding_boundaries(run)
        reference = self.conn.execute((
            'SELECT magnet_up_run_id, magnet_down_run_id '
            'FROM boundary_plots WHERE boundary_run_id = ? AND plot_name = ? '
            'LIMIT 1'
        ), (boundaries[0], plot)).fetchone()
        if not reference:
            return self.reference_run(run, up=up, down=down)
        return reference[0] if up else reference[1]


# List of SQL commands to be executed one-by-one in order whenever the DB is
# loaded, regardless of whether the schema needs instantiating
PRECOMMANDS = [
    'pragma foreign_keys = ON'
]

# Ditionary of table names to table schemas (i.e. defintions)
TABLE_SCHEMAS = {
    'run_boundaries': """
        CREATE TABLE run_boundaries(
            -- Run number of the boundary
            run_id INTEGER PRIMARY KEY CHECK(run_id >= 0),
            -- Magnet up reference run for this run boundary
            magnet_up_run_id INTEGER CHECK(magnet_up_run_id >= 0) NOT NULL,
            -- Magnet up reference run for this run boundary
            magnet_down_run_id INTEGER CHECK(magnet_down_run_id >= 0) NOT NULL

        );
    """,
    'boundary_plots': """
        CREATE TABLE boundary_plots(
            plot_id INTEGER PRIMARY KEY AUTOINCREMENT,
            -- Name of the plot within a run file
            plot_name TEXT NOT NULL,
            -- Boundary this plot overrides references on
            boundary_run_id INTEGER NOT NULL,
            -- Magnet up reference run number to use for this plot
            magnet_up_run_id INTEGER NOT NULL,
            -- Magnet down reference run number to use for this plot
            magnet_down_run_id INTEGER NOT NULL,
            -- Plots overrides references of the boundaries they belong to,
            -- and are deleted when the parent boundary is deleted
            FOREIGN KEY(boundary_run_id) REFERENCES run_boundaries(run_id)
                ON DELETE CASCADE,
            -- A plot can only be overriden once on a boundary
            UNIQUE(boundary_run_id, plot_name)
        );
    """
}
