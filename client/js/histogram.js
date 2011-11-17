
histogram = function(loc,numseries,numbins,min,max,options){
  this.loc = loc;
  this.series = [];
  this.numseries = numseries;
  this.numbins = numbins;
  this.min = min;
  this.max = max;
  this.options = options;
  this.delta = (max-min)/numbins;
  this.numlabels = 4;
  this.sumhist = false;

  if (this.options.sumhist){
    this.numseries = 2;
    this.sumhist = true;
  }

  for (var j=0;j<this.numseries;j++){
    this.series[j] = [];
  }

  for (var i=8;i>3;i--){
    if (this.numbins%i == 0){
      this.numlabels = i;
    }
  }

  this.delta = (this.max-this.min)/this.numbins;
  this.bins_per_label = parseInt(this.numbins/this.numlabels);
  this.ticks = [];
  for (var i=0;i<this.numbins;i++){
    for (var j=0;j<this.numseries;j++){
      this.series[j][i] = [this.min+(i+0.5)*this.delta,0];
    }
    if ((i%this.bins_per_label) == 0){
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
  if ((this.numbins%this.bins_per_label) == 0){
    this.ticks[2*this.numbins] = this.max;
  }
  else{
    this.ticks[2*this.numbins] = [this.max,' '];
  }
  if (this.options.axes){
    if (this.options.axes.xaxis){
      this.options.axes.xaxis.ticks = this.ticks;  
    }
    else{
      this.options.axes.xaxis = {ticks: this.ticks}
    }
  }
  else{
    this.options.axes = {xaxis: {ticks: this.ticks}};
  }

  this.plot = $.jqplot(this.loc,this.series,this.options);
}

histogram.prototype.replot = function(){
  this.plot.replot({resetAxes:true});
}

histogram.prototype.add_items = function(items){
  for (var k=0;k<this.numseries;k++){
    for (var i=0;i<items[k].length;i++){
      for (var j=0;j<this.numbins;j++){
        if ((items[k][i] >= (this.min + j*this.delta)) && (items[k][i] < (this.min + (j+1)*this.delta))){
          this.series[k][j][1]++;
          break;
        }
      }
    }
    this.plot.series[k].data = this.series[k];
  }
}

histogram.prototype.sum_items = function(items){
  if (this.sumhist){
    for (var i=0;i<this.numbins;i++){
      this.series[0][i][1] = this.series[0][i][1] + this.series[1][i][1];
      this.series[1][i][1] = 0;
    }
    for (var i=0;i<items.length;i++){
      for (var j=0;j<this.numbins;j++){
        if ((items[i] >= (this.min + j*this.delta)) && (items[i] < (this.min + (j+1)*this.delta))){
          this.series[1][j][1]++;
          break;
        }
      }
    }
    this.plot.series[0].data = this.series[0];
    this.plot.series[1].data = this.series[1];
  }
}

histogram.prototype.add_series = function(series){
  for (var k=0;k<this.numseries;k++){
    if (series[k].length > 0){
      for (var i=0;i<this.numbins;i++){
        this.series[k][i][1] += series[k][i];
      }
      this.plot.series[k].data = this.series[k];
    }
  }
}

histogram.prototype.sum_series = function(series){
  if (this.sumhist){
    for (var i=0;i<this.numbins;i++){
      this.series[0][i][1] += this.series[1][i][1];
      this.series[1][i][1] = 0;
    }
    for (var i=0;i<this.numbins;i++){
      this.series[1][i][1] = series[i];
    }
    this.plot.series[0].data = this.series[0];
    this.plot.series[1].data = this.series[1];
  }
}

