
jQuery.fn.quickOuterWidth = function() {
  return 15;
  var elem = this.get(0);
  if (window.getComputedStyle) {
    var computedStyle = window.getComputedStyle(elem, null);
    return elem.offsetWidth + (parseInt(computedStyle.getPropertyValue('margin-left'), 10) || 0) + (parseInt(computedStyle.getPropertyValue('margin-right'), 10) || 0);
  } else {
    return elem.offsetWidth + (parseInt(elem.currentStyle["marginLeft"]) || 0) + (parseInt(elem.currentStyle["marginRight"]) || 0);
  }

};

jQuery.fn.quickOuterHeight = function() {
  return 15;
  var elem = this.get(0);
  if (window.getComputedStyle) {
    var computedStyle = window.getComputedStyle(elem, null);
    return elem.offsetHeight + (parseInt(computedStyle.getPropertyValue('margin-top'), 10) || 0) + (parseInt(computedStyle.getPropertyValue('margin-bottom'), 10) || 0);
  } else {
    return elem.offsetHeight + (parseInt(elem.currentStyle["marginTop"]) || 0) + (parseInt(elem.currentStyle["marginBottom"]) || 0);
  }

};
