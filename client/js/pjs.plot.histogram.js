var pjs = (function(pjs,$){
  pjs.plot.histogram = function(loc,name,numBins,min,max,options){
    pjs.plot.call(this,loc,name,numBins,min,max,options);
  }

  pjs.plot.histogram.prototype = new pjs.plot();

  pjs.plot.histogram.prototype.add = function(items,notSum){
    if (this.options.sumHist && !notSum){
      for (var i=0;i<this.numBins;i++){
        this.series[0].bins[i][1] += this.series[1].bins[i][1];
        this.series[1].bins[i][1] = 0;
        this.series[0].data[i] = this.series[0].data[i].concat(this.series[1].data[i]);
        this.series[1].data[i] = [];
      }
      this.series[0].overflow = this.series[0].overflow.concat(this.series[1].overflow);
      this.series[0].underflow = this.series[0].underflow.concat(this.series[1].underflow);
      this.series[1].overflow = [];
      this.series[1].underflow = [];
      for (var i=0;i<items.length;i++){
        for (var j=0;j<this.numBins;j++){
          if (items[i] >= this.max){
            this.series[1].overflow.push(items[i]);
            break;
          }
          if (items[i] < this.min){
            this.series[1].underflow.push(items[i]);
            break;
          }
          if ((items[i] >= (this.min + j*this.delta)) && (items[i] < (this.min + (j+1)*this.delta))){
            this.series[1].bins[j][1]++;
            this.series[1].data[j].push(items[i]);
            break;
          }
        }
      }
      this.plot.series[0].data = this.series[0].bins;
      this.plot.series[1].data = this.series[1].bins;
    }else{
      for (var k=0;k<this.numSeries;k++){
        for (var i=0;i<items[k].length;i++){
          for (var j=0;j<this.numBins;j++){
            if (items[k][i] >= this.max){
              this.series[k].overflow.push(items[k][i]);
              break;
            }
            if (items[k][i] < this.min){
              this.series[k].underflow.push(items[k][i]);
              break;
            }
            if ((items[k][i] >= (this.min + j*this.delta)) && (items[k][i] < (this.min + (j+1)*this.delta))){
              this.series[k].bins[j][1]++;
              this.series[k].data[j].push(items[k][i]);
              break;
            }
          }
        }
        this.plot.series[k].data = this.series[k].bins;
      }
    }
  };


  pjs.plot.histogram.prototype.rebin = function(numBins,min,max){
    this.max = max;
    this.min = min;
    this.delta = (this.max-this.min)/numBins;
    for (var i=0;i<this.numSeries;i++){
      var data = [];
      var tempseries = {bins:[], data:[], overflow: [], underflow: []};
      for (var j=0;j<this.numBins;j++){
        data = data.concat(this.series[i].data[j]);
      }
      data = data.concat(this.series[i].overflow);
      data = data.concat(this.series[i].underflow);
      for (var j=0;j<numBins;j++){
        tempseries.bins[j] = [this.min+(j+0.5)*this.delta,0];
        tempseries.data[j] = [];
      }
      for (var j=0;j<data.length;j++){
        for (var k=0;k<numBins;k++){
          if (data[j] >= this.max){
            tempseries.overflow.push(data[j]);
            break;
          }
          if (data[j] < this.min){
            tempseries.underflow.push(data[j]);
            break;
          }
          if ((data[j] >= (this.min + k*this.delta)) && (data[j] < (this.min + (k+1)*this.delta))){
            tempseries.bins[k][1]++;
            tempseries.data[k].push(data[j]);
            break;
          }
        }
      }
      this.series[i] = tempseries;
    }
    this.numBins = numBins;

    for (var i=8;i>3;i--){
      if (this.numBins%i == 0){
        this.numLabels = i;
      }
    }
    if (this.numBins < 4){
      this.numLabels = this.numBins;
    }
    this.binsPerLabel = parseInt(this.numBins/this.numLabels);
    this.generateTicks();

    this.options.axes.xaxis.ticks = this.ticks;
    var tempSeries = [];
    for (var i=0;i<this.numSeries;i++){
      tempSeries[i] = this.series[i].bins;
    }

    $('#' + this.loc).empty();
    this.plot = null;

    this.plot = $.jqplot(this.loc,tempSeries,this.options);
  };

  return pjs;
}(pjs,jQuery));
