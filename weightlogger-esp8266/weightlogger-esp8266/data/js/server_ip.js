$(function() {
  $.getJSON("/ajax/server").done(function( data ) {
    $("#ipa").val(data.url);
  });
});