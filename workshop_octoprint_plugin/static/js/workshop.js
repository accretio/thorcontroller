
function onmousedownHandler(btn) {
  $(btn).data("clicked", "yes");
  var dir = $(btn).data("dir");
  var device = $(btn).parent().parent().data("device");
  $.post("http://pi1:1025", "device="+device+"&dir="+dir+"&step=1")
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
