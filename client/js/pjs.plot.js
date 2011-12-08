var pjs = (function(pjs,$){
  pjs = (pjs != null) ? pjs : {};

  pjs.plot = function(loc,name,numBins,min,max,options){
    if (loc == null){
      return;
    }
    this.loc = loc;
    this.name = name;
    this.firstKey = -1;
    this.lastKey = -1;
    this.updating = true;
    this.series = [];
    this.numBins = numBins;
    this.min = min;
    this.max = max;
    this.numLabels = 4;
    this.numSeries = 1;

    this.options = (options != null) ? options : {};

    if (this.options.contextMenu){
      this.bindContextMenu();
    }

    if (this.options.sumHist){
      this.numSeries = 2;
    }

    for (var j=0;j<this.numSeries;j++){
      this.series[j] = {bins:[], data:[], overflow:[], underflow:[]};
    }

    for (var i=8;i>3;i--){
      if (this.numBins%i == 0){
        this.numLabels = i;
      }
    }

    if (this.numBins < 4){
      this.numLabels = this.numBins;
    }

    this.delta = (this.max-this.min)/this.numBins;
    this.binsPerLabel = parseInt(this.numBins/this.numLabels);

    if (!this.options.axes){
      this.generateTicks(); 
      this.options.axes = {xaxis: {ticks: this.ticks}};
    }
    else if (!this.options.axes.xaxis){
      this.generateTicks(); 
      this.options.axes.xaxis = {ticks: this.ticks};
    }else if (!this.options.axes.xaxis.ticks){
      this.generateTicks(); 
      this.options.axes.xaxis.ticks = this.ticks;
    }

    for (var i=0;i<this.numBins;i++){
      for (var j=0;j<this.numSeries;j++){
        this.series[j].bins[i] = [this.min+(i+0.5)*this.delta,0];
      }
    }

  };

  pjs.plot.prototype.draw = function(){
    var tempSeries = [];
    for (var i=0;i<this.numSeries;i++){
      tempSeries[i] = this.series[i].bins;
    }
    this.plot = $.jqplot(this.loc,tempSeries,this.options);
  };

  pjs.plot.prototype.update = function(fulldata){
    if (this.updating){
      var plotdata = fulldata[this.name];
      if (plotdata){
        var data = plotdata[1];
        var maxkey = plotdata[0];
        var minkey = maxkey - data.length + 1;

        if (maxkey >= 0){
          if (this.firstKey < 0){
            this.firstKey = minkey;
          }
          this.lastKey = maxkey;
        }

        this.add(data);
        this.replot();
      }
    }
  };



  pjs.plot.prototype.replot = function(){
    this.plot.replot({resetAxes: this.options.resetAxes});
  };

  pjs.plot.prototype.generateTicks = function(){
    this.ticks = [];
    for (var i=0;i<this.numBins;i++){
      for (var j=0;j<this.numSeries;j++){
        this.series[j].data[i] = [];
      }
      if ((i%this.binsPerLabel) == 0){
        this.ticks[2*i] = this.min+i*this.delta;
      }
      else{
        if (this.options.linesPerBin){
          this.ticks[2*i] = [this.min+i*this.delta,' '];
        }
        else{
          this.ticks[2*i] = 0;
        }
      }
      this.ticks[2*i+1] = 0;
    }
    if ((this.numBins%this.binsPerLabel) == 0){
      this.ticks[2*this.numBins] = this.max;
    }
    else{
      this.ticks[2*this.numBins] = [this.max,' '];
    }
  };

  pjs.plot.prototype.clear = function(){
    for (var i=0;i<this.numSeries;i++){
      for (var j=0;j<this.numBins;j++){
        this.series[i].bins[j][1] = 0;
        this.series[i].data[j] = [];
      }
      this.series[i].overflow = [];
      this.series[i].underflow = [];
    }
    this.firstKey = -1;
    var tempSeries = [];
    for (var i=0;i<this.numSeries;i++){
      tempSeries[i] = this.series[i].bins;
    }
    this.replot();
  };

  pjs.plot.prototype.getHistory = function(howmany){
    var hist = this;
    if (this.firstKey != 0){
      $.ajax({
        async: false,
        url: "./data" + "?history=" + this.name + "&startkey=" + (this.firstKey-howmany) + "&endkey=" + (this.firstKey-1),
        dataType:"json",
        success: function(fulldata){
          var data = fulldata[1];
          var maxkey = fulldata[0][1];
          var minkey = fulldata[0][0];

          if (maxkey >= 0){
            if (hist.firstKey < 0 || minkey < hist.firstKey){
              hist.firstKey = minkey;
            }
            if (hist.lastKey < 0){
              hist.lastKey = maxkey;
            }
          }
          hist.add(data);
          hist.replot();
        },
        error: function(jqXHR, textStatus, errorThrown){
          console.log(errorThrown);
        }
      });
    }
  };


  pjs.plot.prototype.bindContextMenu = function(){
    var hist = this;
    $('#' + this.loc).bind('contextmenu',function(e){
      var $cmenu = $(this).next();
      $cmenu.data('plot',hist);
      $('<div class="overlay"></div>').css({left:'0px',top:'0px',position:'absolute',width:'100%',height:'100%',zIndex:'100'}).click(function(){
        $(this).remove();
        $cmenu.hide();
      }).bind('contextmenu',function(){return false;}).appendTo(document.body);
      $(this).next().css({left: e.pageX, top: e.pageY, zIndex: '101'}).show();
      return false;
    });
  };

  pjs.plot.prototype.start = function(){
    this.updating = true;
  };
  
  pjs.plot.prototype.stop = function(){
    this.updating = false;
    this.lastKey = -1;
  };

  return pjs;
}(pjs,jQuery));

