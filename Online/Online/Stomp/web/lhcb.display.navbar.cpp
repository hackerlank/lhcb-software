_loadFile('lhcb.display.general','css');
var HTML_HEAD = new Object();

function setupHTML_HEAD() {
  HTML_HEAD.head = document.getElementsByTagName("head")[0];
  HTML_HEAD.url_base = the_displayObject.url_base;
  HTML_HEAD.url_comet = 'http://lbcomet.cern.ch/static/RunStatus';
  HTML_HEAD.img_base  = 'http://cern.ch/frankm/Online';
}

function setupHTML_BASE(url) {
  var e = document.createElement("base");
  e.setAttribute("href",url);
  HTML_HEAD.head.appendChild(e);
}

var NavigationBar = function() {
  var table = document.createElement('table');
  table.body = document.createElement('tbody');
  table.haveSmallIcons = false;
  table.items = new Array();
  table.icons = null;
  table.className = 'NavigationBar';
  table.body.className = 'NavigationBar';
  table.style.width = '100%';
  table.body.style.width = '100%';
  table.cellSpacing = 2;
  table.addButton = function(text,tooltip,style, action) {
    var tr = document.createElement('tr');
    var td = document.createElement('td');
    td.style.verticalAlign = 'top';
    td.style.textAlign='center';
    td.className = style;
    td.innerHTML = text;
    td.onclick = action;
    td.client = this;
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  }
  
  table.open_abs_url = function(loc) {
    //alert(navigator.appName+'\n\n'+loc);
    doc = document;
    if ( parent.frames["viewer"] != null )  {
      if ( navigator.appName.indexOf("Netscape") != -1 )  {
	doc = parent.frames.viewer;
      }
      else if ( navigator.appName.indexOf("Microsoft") != -1 )  {
	doc = parent.viewer;
      }
      else {
	doc = parent.frames.viewer;
      }
    }
    doc.location = loc;
  }

  table.open_url = function(loc) {
    return this.open_abs_url(HTML_HEAD.url_comet+'/'+loc);
  }

  table.addSized = function(text, tooltip, url, style, small_pic, sx, sy, large_pic, lx, ly) {
    var item     = new Object();
    item.text    = text;
    item.url     = url;
    item.small   = small_pic;
    item.small_height = sy;
    item.small_width  = sx;

    item.large   = large_pic;
    item.large_height = ly;
    item.large_width  = lx;

    item.style        = style;
    item.img          = document.createElement('img');
    item.img.border   = 1;
    item.img.alt      = tooltip;
    //item.img.src      = item.large;
    //item.img.height   = item.large_height;
    //item.img.width    = item.large_width;
    this.items.push(item);
    return this;
  }

  table.add = function(text, tooltip, url, style, small_pic, large_pic) {
    return this.addSized(text, tooltip, url, style, small_pic, 16, 16, large_pic, 32, 32);
  }

  table.setImages = function(small) {
    if ( small ) {
      if ( this.icons ) this.icons.innerHTML = 'Large Icons';
      for (var i=0, len=this.items.length; i < len; ++i)  {
	var item = this.items[i];
	item.img.src = item.small;
	item.img.height = item.small_height;
	item.img.width  = item.small_width;
      }
    }
    else {
      if ( this.icons ) this.icons.innerHTML = 'Small Icons';
      for (var i=0, len=this.items.length; i < len; ++i)  {
	var item = this.items[i];
	item.img.src = item.large;
	item.img.height = item.large_height;
	item.img.width  = item.large_width;
      }
    }
    return this;
  }

  table.build = function() {
    var item, itm, a, br, tr, td;
    for (var i=0, len=this.items.length; i < len; ++i)  {
      item = this.items[i];
      tr = document.createElement('tr');
      td = document.createElement('td');
      td.className = 'NavigationBar';
      td.style.verticalAlign = 'top';
      td.style.textAlign='center';
      td.style.border = 'solid';
      td.style.borderWidth = '1px';
      td.style.borderColor = '#000000';
      
      a = document.createElement('a');
      a.href = item.url;
      if ( item.style ) a.className = item.style;
      a.appendChild(item.img);
      a.appendChild(document.createElement('br'));
      a.appendChild(document.createTextNode(item.text));
      td.appendChild(a);
      tr.appendChild(td);
      this.body.appendChild(tr);
      item.cell = tr;
    }
    this.appendChild(this.body);
    return this.setImages(this.haveSmallIcons);
  }

  table.changeImages = function() {
    this.client.haveSmallIcons = !this.client.haveSmallIcons;
    return this.client.setImages(this.client.haveSmallIcons);
  }

  return table;
}


var navBar = null;


var navbar_unload = function()  {
}

var navbar_body = function()  {
  setupHTML_HEAD();
  setupHTML_BASE(HTML_HEAD.url_base+'/..');
  navBar = NavigationBar();
  navBar.icons = navBar.addButton('Small Icons','Change icon layout', 'DisplayButton', navBar.changeImages);
  navBar.add('Online Home',
	     'Got to the LHCb Online home page',
	     'JavaScript:navBar.open_abs_url("http://lhcb-online.web.cern.ch/lhcb-online")',
	     'NavigationBar',
	     '../Images/Home_16x16.gif',
	     '../Images/Home_32x32.gif');
  navBar.add('Help',
	     'Help',
	     'JavaScript:navBar.open_abs_url("News.htm")',
	     'NavigationBar',
	     '../Images/Help_16x16.gif',
	     '../Images/Help_32x32.gif');

  navBar.add('Page 1',
	     'Show LHCb Page 1',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=page1")',
	     'NavigationBar',
	     '../Images/Search_16x16.gif',
	     '../Images/Search_32x32.gif');

  navBar.add('LHCb Run Status',
	     'Show LHCb run status display',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=status&system=LHCb")',
	     'NavigationBar',
	     '../Images/LookFile_16x16.gif',
	     '../Images/LookFile_32x32.gif');

  navBar.add('FEST Run Status',
	     'Show FEST run status display',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=status&system=FEST")',
	     'NavigationBar',
	     '../Images/Options_16x16.gif',
	     '../Images/Options_32x32.gif');

  navBar.add('Subdetector Run Status',
	     'Show run status display with coice of detector',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=status")',
	     'NavigationBar',
	     '../Images/LookJob_16x16.gif',
	     '../Images/LookJob_32x32.gif');

  navBar.add('Collimators',
	     'Show collimator settings around Point 8',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=collimators")',
	     'NavigationBar',
	     '../Images/LookProd_16x16.gif',
	     '../Images/LookProd_32x32.gif');

  navBar.add('HV Status',
	     'Show LHCb HV status',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=detstatus")',
	     'NavigationBar',
	     '../Images/Wizard_16x16.gif',
	     '../Images/Wizard_32x32.gif');

  navBar.add('BCM',
	     'Show BCM status',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=bcm&charts=1")',
	     'NavigationBar',
	     '../Images/BCM.png',
	     '../Images/BCM.png');

  navBar.add('Magnet',
	     'Show Magnet status',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=magnet")',
	     'NavigationBar',
	     '../Images/LHCb/Magnet_Icon.png',
	     '../Images/LHCb/Magnet_Icon.png');

  navBar.addSized('LHC Status',
		  'LHC status around LHCb',
		  'JavaScript:navBar.open_url("lhcb.display.htm?type=lhc")',
		  'NavigationBar',
  		  '../Images/Beams.jpg',16,16,
  		  '../Images/Beams.jpg',32,32);
  //		  '../Images/LHC3.jpg',24,16,
  //		  '../Images/LHC3.jpg',48,32);
  navBar.addSized('Elog',
		  'LHCb electronic logbook',
		  'JavaScript:navBar.open_abs_url("http://lblogbook.cern.ch/Shift/")',
		  'NavigationBar',
		  '../Images/logbook.png',16,16,
		  '../Images/logbook.png',32,32);
  /*
  navBar.addSized('Ramses',
		  'Radiation Monitoring in LHCb',
		  'JavaScript:navBar.open_url("lhcb.display.htm?type=ramses")',
		  'NavigationBar',
		  '../Images/LHCb/Ramses_icon.jpg',20,20,
		  '../Images/LHCb/Ramses_icon.jpg',32,32);
  */
  navBar.addSized('LHCb',
		  'LHCb home page',
		  'JavaScript:navBar.open_abs_url("http://lhcb.cern.ch")',
		  'NavigationBar',
		  'http://lhcb.cern.ch/lhcblogo.gif',64,32,
		  'http://lhcb.cern.ch/lhcblogo.gif',64,32);
  
  navBar.addSized('LHC',
		  'LHC collider home page',
		  'JavaScript:navBar.open_abs_url("http://cern.ch/lhc")',
		  'NavigationBar',
		  'http://lhc.web.cern.ch/lhc/images/LHC.gif',32,32,
		  'http://lhc.web.cern.ch/lhc/images/LHC.gif',48,48);
  navBar.addSized('Operations',
		  'CERN collider operations home page',
		  'JavaScript:navBar.open_abs_url("http://op-webtools.web.cern.ch/op-webtools/vistar/vistars.php?usr=SPS1")',
		  'NavigationBar',
		  '../Images/BeamsDep.jpg',80,16,
		  '../Images/BeamsDep.jpg',120,32);
  navBar.addSized('CERN',
		  'CERN home page',
		  'JavaScript:navBar.open_abs_url("http://cern.ch")',
		  'NavigationBar',
		  'http://user.web.cern.ch/User/zCommonImages/CERNLogo.png',32,32,
		  'http://user.web.cern.ch/User/zCommonImages/CERNLogo.png',48,48);
  
  navBar.build();
  HTML_HEAD.body = document.getElementsByTagName('body')[0];
  HTML_HEAD.body.appendChild(navBar);
  HTML_HEAD.body.className = 'NavigationBar';
  HTML_HEAD.body.style.width = '100%';
  HTML_HEAD.body.style.backgroundColor = '#666666';
  HTML_HEAD.body.style.color = '#FFFFFF';
  return navBar;
}

if ( _debugLoading ) alert('Script lhcb.display.navbar.cpp loaded successfully');
