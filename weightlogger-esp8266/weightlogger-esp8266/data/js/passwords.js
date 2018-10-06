if (!String.prototype.trim) {
  (function() {
      // Make sure we trim BOM and NBSP
      var rtrim = /^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g;
      String.prototype.trim = function() {
          return this.replace(rtrim, '');
      };
  })();
}

function checkConfirmations() {
  var p_remot = $("#p_remot").val();
  var p_remot_conf = $("#p_remot_conf").val();
  var p_admin = $("#p_admin").val();
  var p_admin_conf = $("#p_admin_conf").val();
  if (p_admin.trim().length == 0 && p_remot.trim().lenght == 0) {
    return false;
  }
  if (p_admin!="" && p_admin != p_admin_conf) {
    alert("Password and confirmation for ADMIN don't match");
    return false;
  }
  if (p_remot!="" && p_remot != p_remot_conf) {
    alert("Password and confirmation for REMOTE don't match");
    return false;
  }
  $("#pconf").val(0);
  return true;
}