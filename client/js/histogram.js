
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
    this.series[j] = {bins:[], data:[]};
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
      this.series[j].bins[i] = [this.min+(i+0.5)*this.delta,0];
      this.series[j].data[i] = [];
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

  var temp_series = [];
  for (var i=0;i<this.numseries;i++){
    temp_series[i] = this.series[i].bins;
  }
  this.plot = $.jqplot(this.loc,temp_series,this.options);
}

histogram.prototype.replot = function(){
  this.plot.replot({resetAxes:true});
}

histogram.prototype.add_items = function(items){
  for (var k=0;k<this.numseries;k++){
    for (var i=0;i<items[k].length;i++){
      for (var j=0;j<this.numbins;j++){
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

histogram.prototype.sum_items = function(items){
  if (this.sumhist){
    for (var i=0;i<this.numbins;i++){
      this.series[0].bins[i][1] = this.series[0].bins[i][1] + this.series[1].bins[i][1];
      this.series[1].bins[i][1] = 0;
      this.series[0].data[i] = this.series[0].data[i].concat(this.series[1].data[i]);
      this.series[1].data[i] = [];
    }
    for (var i=0;i<items.length;i++){
      for (var j=0;j<this.numbins;j++){
        if ((items[i] >= (this.min + j*this.delta)) && (items[i] < (this.min + (j+1)*this.delta))){
          this.series[1].bins[j][1]++;
          this.series[1].data[j].push(items[i]);
          break;
        }
      }
    }
    this.plot.series[0].data = this.series[0].bins;
    this.plot.series[1].data = this.series[1].bins;
  }
}

histogram.prototype.add_series = function(series){
  for (var k=0;k<this.numseries;k++){
    if (series[k].length > 0){
      for (var i=0;i<this.numbins;i++){
        this.series[k].bins[i][1] += series[k][i];
      }
      this.plot.series[k].data = this.series[k].bins;
    }
  }
}

histogram.prototype.sum_series = function(series){
  if (this.sumhist){
    for (var i=0;i<this.numbins;i++){
      this.series[0].bins[i][1] += this.series[1].bins[i][1];
      this.series[1].bins[i][1] = 0;
    }
    for (var i=0;i<this.numbins;i++){
      this.series[1].bins[i][1] = series[i];
    }
    this.plot.series[0].data = this.series[0].bins;
    this.plot.series[1].data = this.series[1].bins;
  }
}

histogram.prototype.rebin = function(numbins){
  this.delta = (this.max-this.min)/numbins;
  for (var i=0;i<this.numseries;i++){
    var data = [];
    var tempseries = {bins: [], data: []};
    for (var j=0;j<this.numbins;j++){
      data = data.concat(this.series[i].data[j]);
    }
    console.log(data);
    for (var j=0;j<numbins;j++){
      tempseries.bins[j] = [this.min+(j+0.5)*this.delta,0];
      tempseries.data[j] = [];
    }
    for (var j=0;j<data.length;j++){
      for (var k=0;k<numbins;k++){
        if ((data[j] >= (this.min + k*this.delta)) && (data[j] < (this.min + (k+1)*this.delta))){
          tempseries.bins[k][1]++;
          tempseries.data[k].push(data[j]);
          break;
        }
      }
    }
    this.series[i] = tempseries;
  }
  this.numbins = numbins;

  for (var i=8;i>3;i--){
    if (this.numbins%i == 0){
      this.numlabels = i;
    }
  }
  this.bins_per_label = parseInt(this.numbins/this.numlabels);
  this.ticks = [];
  for (var i=0;i<this.numbins;i++){
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

  console.log(this.series);
  console.log(this.ticks);

  this.options.axes = {xaxis: {ticks: this.ticks}};
  var temp_series = [];
  for (var i=0;i<this.numseries;i++){
    temp_series[i] = this.series[i].bins;
  }

  $('#' + this.loc).empty();
  this.plot = null;

  this.plot = $.jqplot(this.loc,temp_series,this.options);
}

