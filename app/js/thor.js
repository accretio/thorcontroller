
function onmousedownHandler(btn) {
  $(btn).data("clicked", "yes");
  var dir = $(btn).data("dir");
  var motor = $(btn).parent().parent().data("motor");
  $.post("http://pi1:1025", "motor="+motor+"&dir="+dir+"&step=1")
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
