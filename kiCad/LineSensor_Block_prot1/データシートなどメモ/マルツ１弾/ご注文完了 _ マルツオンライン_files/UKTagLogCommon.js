/**
 * ユニバーサルナレッジ タグログ（共通部分）
 * 履歴   : WebSystemGenerator-latest, Created
 * @since 2024/02/14
 */

var _ukwqcommon = [];
var _ukwhost = 'uabizprd.ukw.jp';

_ukwqcommon.push(['_setClient','marutsu']);

_ukwqcommon.push(['_setReqURL',location.href]);
_ukwqcommon.push(['_setRefURL',document.referrer]);

//
if (navigator.userAgent.match(/iPhone|Android.+Mobile/)) {
	_ukwqcommon.push(['_setDisplay','sp']);
} else {
	_ukwqcommon.push(['_setDisplay','pc']);
}

function sendTagLog() {

	var _ukt = document.createElement('script'); _ukt.type = 'text/javascript'; _ukt.async = true;
	_ukt.src = '//' + _ukwhost + '/taglog/ukwlg.js';
	var _uks = document.getElementsByTagName('script')[0]; _uks.parentNode.insertBefore(_ukt,_uks);

}