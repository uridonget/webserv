document.addEventListener("DOMContentLoaded", function() {
    var video = document.getElementById("myVideo");
    var playPauseBtn = document.getElementById("playPauseBtn");
    var stopBtn = document.getElementById("stopBtn");
    var muteBtn = document.getElementById("muteBtn");

    playPauseBtn.addEventListener("click", function() {
        if (video.paused) {
            video.play();
            playPauseBtn.textContent = "일시정지";
        } else {
            video.pause();
            playPauseBtn.textContent = "재생";
        }
    });

    stopBtn.addEventListener("click", function() {
        video.pause();
        video.currentTime = 0;
        playPauseBtn.textContent = "재생";
    });

    muteBtn.addEventListener("click", function() {
        video.muted = !video.muted;
        muteBtn.textContent = video.muted ? "음소거 해제" : "음소거";
    });
});