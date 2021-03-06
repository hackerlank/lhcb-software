//--------------------------------------------------------------------------------------------------------------------------
if ( !_lhcb().desktop ) {
  lhcb.desktop = function() {

    this.make_iframe = function(url)  {
      var ifr='<iframe id="myFrame" frameborder="0" vspace="0" hspace="0" marginwidth="0" marginheight="0" width="100%" height="100%" src="'+url+'" scrolling="auto" style="overflow:visible"></iframe>';
      return ifr;
    };

    this._makeIcon = function(img) {
      if ( img ) {
	if ( img.iconSize ) {
	  return '<img src="'+img.icon+'" width="'+img.iconSize.small.width+'" height="'+img.iconSize.small.height+'"/>';
	}
	return '<img src="'+img.icon+'" width="16" height="16"/>';
      }
      return '';
    };

    this._makeIconSize = function(img,size) {
      if ( size ) {
	return '<img src="'+img.icon+'" width="'+size.width+'" height="'+size.height+'"/>';
      }
      return lhcb.desktop._makeIcon(img);
    };

    this.URLModule = Ext.extend(Ext.app.Module, {
      url:        lhcb.constants.urls.empty,
      init:       function(){},
      _createWin: function()  {
	var desktop = this.app.getDesktop();
	var win = desktop.getWindow(this.url.nick);
	if(!win)   {
	  win = desktop.createWindow({
                                 id:              this.url.nick,
				 title:           this.url.title,
				 width:           this.url.size.width,
				 height:          this.url.size.height,
				 initHidden:      true,
				 html:            lhcb.desktop.make_iframe(this.url.src),
				 iconCls:         this.url.iconCls,
				 shim:            false,
				 animCollapse:    false,
				 autoScroll:      true,
				 layout:          'fit',
				 constrainHeader: true
						    });
	}
	win.show();
      },

      _launch: function(url) {
	this.url            = url;
	this.id             = url.nick;
        this.tooltip        = url.nick+'\n'+url.title,
	this.createWindow   = function(src) {this._createWin();};
	this.launcher       = {text:           this.url.title,
			       iconCls:        this.url.iconCls,
			       shortcutIconCls:this.url.iconCls,
                               tooltip:        'Click to move to the requested page:\n'+this.url.title+'\n'+this.url.src,
			       handler:        this.createWindow,
			       scope:          this
	};
      }
    });

    this.menuWindowIndex = 0;

    this.URLMenuModule = Ext.extend(Ext.app.Module, {
      iconCls:    '',
      img:        lhcb.constants.images.bogus,
      moduleType: 'menu',
      init: function()  {
	this.launcher = {
	  text:     'Window '+(++lhcb.desktop.menuWindowIndex),
	  iconCls:  'bogus',
	  handler:   this.createWindow,
	  windowId:  lhcb.desktop.menuWindowIndex,
	  scope:     this
	}
      },
      _launchSize: function(title,img,items,size) {
	this.img = img;
	if ( this.img ) {
	  this.text    = title;//lhcb.desktop._makeIconSize(this.img,size)+'&nbsp;'+title;
	  this.iconCls = 'bogus';
	}
	else {
	  this.text    = title;
	  this.iconCls = 'bogus';
	}
	this.handler = function() { return false;  };
	this.launcher = {
	  text:    this.text,
	  title:   this.text,
	  iconCls: this.iconCls,
	  handler: this.createWindow,
	  scope:   this,
	  menu: {  items:  items  }
	};
	},
	  _launch: function(title,img,items) {
	  this._launchSize(title,img,items,null);
	},
      createWindow: function(url){
	var desktop = this.app.getDesktop();
	var win = desktop.getWindow(url.id);
	if(!win){
	  win = desktop.createWindow({
	    id:              url.id,
		title:           url.title,
		width:           url.size.width,
		height:          url.size.height,
		initHidden:      true,
		html:            lhcb.desktop.make_iframe(url.src),
		iconCls:         url.iconCls,
		shim:            false,
		animCollapse:    false,
		autoScroll:      true,
		//layout:          'fit',
		constrainHeader: true
		});
	}
	win.show();
      },
      menuItemId: function(title,id,source,width,height)   {
	var url = {
	  id:       id,
          src:      source,
	  text:     title,
	  nick:     title,
	  title:    title,
	  size:   { width: width, height: height },
	  iconCls:  'bogus',
	  handler:  this.createWindow,
	  scope:    this,
	  windowId: ++lhcb.desktop.menuWindowIndex,
	  makeIcon: function(img) {
	    this.text = _makeIcon(img)+'&nbsp;'+this.title;
	    return this;
	  }
	};
	return url;
      },
      menuItem: function(title,url,width,height)   {
	return this.menuItemId(title,title,url,width,height);
      },
      menuURL: function(url)   { 
	return this.menuItemId(url.title,url.nick,url.src,url.size.width,url.size.height);
      }
    });

    this.shortcut = function(url) {
      var w = 32, h = 32;
      var cut = document.createElement('dt');
      var ref = document.createElement('a');
      ref.href = '#';
      cut.id = url.nick+'-shortcut';
      if ( url.img.iconSize )  {
	w = url.img.iconSize.large.width;
	h = url.img.iconSize.large.height;
      }
      ref.innerHTML=url.title;//'<img src="'+url.img.src+'" width="'+w+'" height="'+h+'"/><div>'+url.title+'</div>';
      cut.appendChild(ref);
      return cut;
    };

    this.defineLogo = function(url,target) {
      var p = document.getElementById('x-desktop');
      var a = document.createElement('a');
      a.href   = target;
      a.target = '_blank';
      a.innerHTML    = '<img style="float:right;margin:15px;" src="'+url+'" width="80"/>';
      p.appendChild(a);
      return p;
    };

    return this;
  }();
}
