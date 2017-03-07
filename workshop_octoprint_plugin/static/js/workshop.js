

function onmousedownHandler(btn) {
  $(btn).data("clicked", "yes");
  var dir = $(btn).data("dir");
  var device = $(btn).parent().parent().data("device");
  $.post("/controller/operate", "device="+device+"&dir="+dir+"&step=1")
    .done(function( data ) {
      console.log(data);
      if ($(btn).data("clicked") == "yes") {
        onmousedownHandler(btn);
      }
    })
  
}

function onmouseupHandler(btn) {
  $(btn).data("clicked", "no");
}

function activate(btn) {
  console.log("activate");
  $.post("/controller/admin", "active=1")
    .done(function(data) { 
      $(btn).addClass("btn-success");
      $("#deactivateBtn").removeClass("btn-danger");
      
    })
}

function deactivate(btn) {
  console.log("deactivate");
  $.post("/controller/admin", "active=0")
    .done(function(data) {
      $("#activateBtn").removeClass("btn-success");
      $(btn).addClass("btn-danger");
    })
}

