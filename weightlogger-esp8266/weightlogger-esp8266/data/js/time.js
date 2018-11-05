$( function() {
  $( "#datepicker" ).datepicker({
    showOtherMonths: true,
    selectOtherMonths: true,
    dateFormat: "yy-mm-dd"
  });
  $('#timepicker').timepicker({ 'timeFormat': 'H:i:s' })
});