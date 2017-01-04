// ==UserScript==
// @name        Reddit BTC Ticker
// @namespace   topherkhaew
// @include     https://www.reddit.com/r/Bitcoin/*
// @require     http://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js
// @version     1
// @grant       GM_xmlhttpRequest
// ==/UserScript==

function add_btc_price(price) {
  var btc_css = {
      "position": "absolute",
      "top": "60px",
      "right": "60px",
      "font-size": "14pt"
    };
    var btc = jQuery("<div>"+parseFloat(price).toFixed(2)+"</div>").appendTo("#header");
    btc.css(btc_css);
}

GM_xmlhttpRequest ({
  method: "GET",
  url: "https://blockchain.info/ticker",
  onload: function (response) { 
    data = $.parseJSON(response.responseText);
    add_btc_price(data['USD']['last']);
    }
});
  