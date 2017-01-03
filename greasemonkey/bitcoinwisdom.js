// ==UserScript==
// @name        Tweak BitcoinWisdom
// @namespace   topherkhaew
// @description Tweak some stuff in bitcoinwisdom.com
// @include     https://bitcoinwisdom.com/*
// @version     1
// @grant       none
// ==/UserScript==

document.getElementById("leftbar").style.display = "none";

var last = "";
var MutationObserver = window.MutationObserver || window.WebKitMutationObserver || window.MozMutationObserver;
var observer = new MutationObserver(function(mutations) {
    mutations.forEach(function(mutation) {
        if(last == mutation.target.textContent)
            return;
        if($symbol.indexOf("cny") !== -1) {
           usd = (parseFloat(mutation.target.textContent)/$c_usdcny).toFixed(2);
           last = mutation.target.textContent + " = " + usd;
           mutation.target.textContent = last;
           }
        });
});
observer.observe(document.getElementById("price"), { childList: true });


