/**
 * カートリカバリー（共通部分）
 * 履歴   : WebSystemGenerator-latest, Created
 * @since 2024/12/18
 */

var _crTracking = "CTX-Thbn445f";
const _crHost = window.location.hostname;
if (_crHost != "www.marutsu.co.jp") {
	_crTracking += "-TEST";
}

function sendCartRecovery() {

	(function(w,d,s,c,i){w[c]=w[c]||{};w[c].trackingId=i;
	var f=d.getElementsByTagName(s)[0],j=d.createElement(s);j.async=true;
	j.src='//cdn.contx.net/collect.js';f.parentNode.insertBefore(j,f);
	})(window,document,'script','contx', _crTracking);

}