var pjs = (function(pjs,$){
  pjs.plot.barGraph = function(loc,name,numBins,min,max,options){
    pjs.plot.call(this,loc,name,numBins,min,max,options);
  }
 
  pjs.plot.barGraph.prototype = new pjs.plot();

  pjs.plot.barGraph.prototype.add = function(series,notSum){
    if (this.options.sumHist && !notSum){
      for (var i=0;i<this.numBins;i++){
        this.series[0].bins[i][1] += this.series[1].bins[i][1];
        this.series[1].bins[i][1] = 0;
      }
      for (var i=0;i<this.numBins;i++){
        for (var j=0;j<series.length;j++){
          this.series[1].bins[i][1] += series[j][i];
        }
      }
      this.plot.series[0].data = this.series[0].bins;
      this.plot.series[1].data = this.series[1].bins;
    }else{
      for (var k=0;k<this.numSeries;k++){
        if (series[k].length > 0){
          for (var i=0;i<this.numBins;i++){
            this.series[k].bins[i][1] += series[k][i];
          }
          this.plot.series[k].data = this.series[k].bins;
        }
      }
    }
  };

  return pjs;
}(pjs,jQuery));
