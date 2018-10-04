$(document).ready(function(){
  $.getJSON("/ajax/server_ip", function (data){
    $("#ipa").val(data.ip);
    $("#port").val(data.port);
  });
});