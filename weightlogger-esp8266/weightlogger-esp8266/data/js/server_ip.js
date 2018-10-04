$(function() {
  $.getJSON("/ajax/server").done(function( data ) {
    $("#ipa").val(data.ip);
    $("#port").val(data.port);
  });
});