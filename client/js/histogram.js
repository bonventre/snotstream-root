
histogram = function(loc,name,numseries,numbins,min,max,options){
  this.loc = loc;
  this.series = [];
  this.numseries = numseries;
  this.numbins = numbins;
  this.min = min;
  this.max = max;
  this.options = options;
  this.delta = (max-min)/numbins;
  this.numlabels = 4;
  this.firstkey = -1;
  this.lastkey = -1;
  this.updating = true;
  this.name = name;
  this.sumhist = false;
  this.fixedbins = false;
  this.linesperbin = false;

  if (this.options.contextMenu){
    this.bind_context_menu();
  }

  if (this.options.fixedBins){
    this.fixedbins = true;
  }

  if (this.options.linesPerBin){
    this.linesperbin = true;
  }

  if (this.options.sumHist){
    this.numseries = 2;
    this.sumhist = true;
  }

  for (var j=0;j<this.numseries;j++){
    this.series[j] = {bins:[], data:[], overflow: [], underflow: []};
  }

  for (var i=8;i>3;i--){
    if (this.numbins%i == 0){
      this.numlabels = i;
    }
  }
  if (this.numbins < 4){
    this.numlabels = this.numbins;
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
      if (this.linesperbin){
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
      if (this.options.axes.xaxis.ticks){
      }
      else{
        this.options.axes.xaxis.ticks = this.ticks;  
      }
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
  this.plot.replot({resetAxes:["yaxis"]});
}

histogram.prototype.add_items = function(items){
  for (var k=0;k<this.numseries;k++){
    for (var i=0;i<items[k].length;i++){
      for (var j=0;j<this.numbins;j++){
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

histogram.prototype.sum_items = function(items){
  if (this.sumhist){
    for (var i=0;i<this.numbins;i++){
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
      for (var j=0;j<this.numbins;j++){
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
      for (var j=0;j<series.length;j++){
        this.series[1].bins[i][1] = series[j][i];
      }
    }
    this.plot.series[0].data = this.series[0].bins;
    this.plot.series[1].data = this.series[1].bins;
  }
}

histogram.prototype.rebin = function(numbins,min,max){
  if (this.fixedbins == false){
  this.max = max;
  this.min = min;
  this.delta = (this.max-this.min)/numbins;
  for (var i=0;i<this.numseries;i++){
    var data = [];
    var tempseries = {bins:[], data:[], overflow: [], underflow: []};
    for (var j=0;j<this.numbins;j++){
      data = data.concat(this.series[i].data[j]);
    }
    data = data.concat(this.series[i].overflow);
    data = data.concat(this.series[i].underflow);
    for (var j=0;j<numbins;j++){
      tempseries.bins[j] = [this.min+(j+0.5)*this.delta,0];
      tempseries.data[j] = [];
    }
    for (var j=0;j<data.length;j++){
      for (var k=0;k<numbins;k++){
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
  this.numbins = numbins;

  for (var i=8;i>3;i--){
    if (this.numbins%i == 0){
      this.numlabels = i;
    }
  }
  if (this.numbins < 4){
    this.numlabels = this.numbins;
  }
  this.bins_per_label = parseInt(this.numbins/this.numlabels);
  this.ticks = [];
  for (var i=0;i<this.numbins;i++){
    if ((i%this.bins_per_label) == 0){
      this.ticks[2*i] = this.min+i*this.delta;
    }
    else{
      if (this.linesperbin){
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


  this.options.axes.xaxis.ticks = this.ticks;
  var temp_series = [];
  for (var i=0;i<this.numseries;i++){
    temp_series[i] = this.series[i].bins;
  }

  $('#' + this.loc).empty();
  this.plot = null;

  this.plot = $.jqplot(this.loc,temp_series,this.options);
  }
}

histogram.prototype.json_update = function(fulldata){
  if (this.updating){
    fulldata = fulldata[this.name];
    if (fulldata){
      var data = fulldata[1];
      var maxkey = fulldata[0];
      var minkey = maxkey - data.length + 1;

      if (maxkey >= 0){
        if (this.firstkey < 0){
          this.firstkey = minkey;
        }
        this.lastkey = maxkey;
      }

      if (this.fixedbins == false){
        this.sum_items(data);
      }else{
        this.sum_series(data);
      }
      this.replot();
    }
  }
}

histogram.prototype.get_history = function(howmany){
  var hist = this;
  if (this.firstkey != 0){
    $.ajax({
      async: false,
      url: "./data" + "?history=" + this.name + "&startkey=" + (this.firstkey-howmany) + "&endkey=" + (this.firstkey-1),
      dataType:"json",
      success: function(fulldata){
        var data = fulldata[1];
        var maxkey = fulldata[0][1];
        var minkey = fulldata[0][0];

        if (maxkey >= 0){
          if (hist.firstkey < 0 || minkey < hist.firstkey){
            hist.firstkey = minkey;
          }
          if (hist.lastkey < 0){
            hist.lastkey = maxkey;
          }
        }
        if (hist.fixedbins == false){
          hist.sum_items(data);
        }
        else{
          hist.sum_series(data);
        }
        hist.replot();
      },
      error: function(jqXHR, textStatus, errorThrown){
        console.log(errorThrown);
      }
    });
  }
}

histogram.prototype.clear = function(){
  for (var i=0;i<this.numseries;i++){
    for (var j=0;j<this.numbins;j++){
      this.series[i].bins[j][1] = 0;
      this.series[i].data[j] = [];
    }
    this.series[i].overflow = [];
    this.series[i].underflow = [];
  }
  this.firstkey = -1;
  this.plot.series[0].data = this.series[0].bins;
  this.plot.series[1].data = this.series[1].bins;
  this.replot();
}

histogram.prototype.bind_context_menu = function(){
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
}

