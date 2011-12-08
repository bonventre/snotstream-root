var pjs = (function(pjs,$){
  pjs.plot.rateGraph = function(loc,name,numBins,min,max,options){
    options = (options != null) ? options : {};
    options.sumHist = false;
    pjs.plot.call(this,loc,name,numBins,min,max,options);
    this.firstClock = -1;
    this.lastClock = -1;
    this.series[0].log = [];
    for (var i=0;i<this.numBins;i++){
      this.series[0].data[i] = 0;
    }
  }
 
  pjs.plot.rateGraph.prototype = new pjs.plot();

  pjs.plot.rateGraph.prototype.update = function(fulldata){
    if (this.updating){
      var plotdata = fulldata[this.name];
      if (plotdata){
        var data = plotdata[1];
        var maxkey = plotdata[0];
        var minkey = maxkey - data.length + 1;
        if (data.length > 0){
          if (this.firstClock < 0){
            this.firstClock = data[0][1];
          }
          this.lastClock = data[data.length-1][1];
        }

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


  pjs.plot.rateGraph.prototype.add = function(series){
    for (var i=0;i<series.length;i++){
      for (var j=0;j<this.numBins;j++){
        this.series[0].data[j] += series[i][0][j];
      }
    }
    this.series[0].log = this.series[0].log.concat(series);
    if (this.options.rollTime){
      var oldtime = this.firstClock;
      this.firstClock = this.lastClock - (this.options.rollTime*10000000);
      if (oldtime > this.firstClock){
        this.firstClock = oldtime;
      }
      for (var i=0;i<this.series[0].log.length;i++){
        if (this.series[0].log[i][1] < this.firstClock && this.series[0].log[i][1] >= oldtime){
          for (var j=0;j<this.numBins;j++){
            this.series[0].data[j]-= this.series[0].log[i][0][j];
          }
        }
      }
    }
    for (var i=0;i<this.numBins;i++){
      if (this.lastClock != this.firstClock){
        this.series[0].bins[i][1] = this.series[0].data[i]/((this.lastClock-this.firstClock)/10000000);
      }else{
        this.series[0].bins[i][1] = 0;
      }
    }

    this.plot.series[0].data = this.series[0].bins;
  };

  pjs.plot.rateGraph.prototype.clear = function(){
    pjs.plot.prototype.clear.call(this);
    this.firstClock = -1;
    this.lastClock = -1;
    this.series[0].log = [];
    for (var i=0;i<this.numBins;i++){
      this.series[0].data[i] = 0;
    }
  };


  return pjs;
}(pjs,jQuery));
