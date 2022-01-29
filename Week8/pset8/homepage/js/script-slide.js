// I found this JQuery script in https://www.sitepoint.com/community/t/js-jquery-animation-for-caption-outside-bootstrap-carousel/354732 so I can manage the captions outside the carousel slides.

$(function () {
    $("#carouselExampleCaptions").carousel();

    var caption = $("div.carousel-item:nth-child(1) .carousel-caption");
    $("#subs_carousel span").html(caption.html());
    caption.css("display", "none");

    $("#carouselExampleCaptions").on("slide.bs.carousel", function (evt) {
      var caption = $(
        "div.carousel-item:nth-child(" +
          ($(evt.relatedTarget).index() + 1) +
          ") .carousel-caption"
      );
      $("#subs_carousel span").html(caption.html());

      $("#subs_carousel span").removeClass("activeLeft activeRight");
      caption.css("display", "none");
      $("#subs_carousel span")[0].offsetWidth; //trigger reflow to start keyframe
      if (evt.direction === "left") {
        $("#subs_carousel span").addClass("activeLeft");
      }
      if (evt.direction === "right") {
        $("#subs_carousel span").addClass("activeRight");
      }
    });
  });