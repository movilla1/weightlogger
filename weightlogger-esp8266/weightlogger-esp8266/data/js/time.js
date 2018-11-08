Number.prototype.pad = function(size) {
  var s = String(this);
  while (s.length < (size || 2)) {s = "0" + s;}
  return s;
}

(function( $ ) {
  $.fn.options_number_range = function(min, max) {
    for (i = min; i <= max; i++) {
      padded_num = i < 10 ? "0" + i : i;
      opt = new Option(padded_num, padded_num);
      $(this).append($(opt));
    }
  };
}( jQuery ));


$( function() {
  $( "#datepicker" ).datepicker({
    showOtherMonths: true,
    selectOtherMonths: true,
    dateFormat: "yy-mm-dd"
  });
  $("#hours").options_number_range(0, 23);
  $("#minutes").options_number_range(0, 59);
  $("#seconds").options_number_range(0, 59);
});

