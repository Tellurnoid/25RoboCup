// KeybordTrap
var chk=0;
var fnKeyName= new Array("F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12")
var fnKeyCode= new Array(113,114,115,116,117,118,119,120,121,122,123)
var lastKeyCode = 0;

var KEY_NONE = 0;			// Keyイベントの取り消し
var KEY_BS = 8;				// BackSpace
var KEY_TAB = 9;			// TAB
var KEY_ENTER = 13;			// Enter

var isInitFocus = true;

/*********************************************************************************************************************************************/
/**  Body初期処理
/*********************************************************************************************************************************************/
function isIE() {
	if (isLaterThanEqualIE10()) {
		return true;
	}
	
	var ua = navigator.userAgent.toLowerCase();
	if (ua.indexOf("trident") >= 0) {
		// IE11を考慮した判定
		return true;
	}

	return false;
}

/**
 * ブラウザがIEかの判定をします。(バージョン10まで判定可能)
 */
function isLaterThanEqualIE10() {
	if (navigator.appName.charAt(0) == "M") {
		return true;
	} else {
		return false;
	}
}

function initBody(){
	initializeScreenStatus();

	// イベントハンドラの設定
	initEvent();
}

function initializeScreenStatus() {
	var obj;

	// キーが押された時
	document.onkeydown = eventKeyDown;

	var objForm = document.forms;
	var isFocused = false;
	var isExistErrorMessage = Validator.isExistErrorMessage();
	for (var i = 0 ; i < objForm.length ; i++) {
		if (!isExistErrorMessage) {
			// エラーメッセージの先頭に後程(タブの初期化などが完了した段階で)フォーカスさせるので、先頭フォーカスはしない
			if (!isFocused) {
				isFocused = focusEnabledFirst(objForm[i].elements);
			}
		}
			
		for (var n = 0 ; n < objForm[i].elements.length; n++) {
			obj = objForm[i].elements[n];
   			if (obj.type == 'text' || obj.type == 'radio' || obj.type == 'checkbox' || obj.type == 'select-one' || obj.type == 'select-multiple' || obj.type == 'textarea' ||  obj.type == 'password' || obj.type == 'button' || obj.type == 'submit' || obj.type == 'reset') {
    			obj.onkeydown = eventEntry;
    			obj.initValue = obj.value;
			}
		}
	}
}

function focusEnabledFirst(elements) {
	if (!isInitFocus) {
		return false;
	}

	if (isNullorUndefined(elements)) {
		return false;
	}
	
	for (var n = 0 ; n < elements.length; n++) {
		obj = elements[n];
		if (isNullorUndefined(obj)) {
			continue;
		}
		
		if (obj.tabIndex > 0 && !obj.disabled){
			if (isObjectInputType(obj)) {
				obj.focus();
				saveFocusValue(obj)
				return true;
			}
		}
	}
	
	return false;
}

function isObjectInputType(obj) {
	if (isNullorUndefined(obj)) {
		return false;
	}
	if (obj.type == 'text' || obj.type == 'radio' || obj.type == 'checkbox' || obj.type == 'select-one' || obj.type == 'select-multiple' || obj.type == 'textarea' ||  obj.type == 'password') {
		return true;
	} else {
		return false;
	}
}

// イベントハンドラの設定
function initEvent() {
	// Formオブジェクト単位のフォーカスハンドラ設定
	var objForms = document.forms;
	for(var i=0 ; i<objForms.length ; i++) {
		for(var j=0 ; j<objForms[i].elements.length ; j++) {
			var obj = objForms[i].elements[j];

			// disabledやreadonlyの場合は対象外
			if (isObjectReadOnly(obj)) {
				continue;
			}

			// 自動埋め込みを行わないオブジェクトは対象外
			if (obj.className.indexOf("NON_AUTO") >= 0) {
				continue;
			}
			
			// イベント設定
			setEventListener(obj);
		}
	}

	// IE以外はDIVも
	if (!isIE()) {
		var divs = document.getElementsByTagName("DIV");
		for(var i=0 ; i<divs.length ; i++) {
			if (divs[i].className.indexOf("SCROLL_TABLE") >= 0) {
				makeFixHeader(divs[i]);
			}
		}
	}
}

function isObjectReadOnly(obj) {
	if (isNullorUndefined(obj)) {
		return true;
	}
	if (obj.disabled || obj.readOnly) {
		return true;
	} else {
		return false;
	}
}

function setEventListener(obj) {
	if (obj.type == 'text' || obj.type == 'textarea' ||  obj.type == 'password' || obj.type == 'select-one' || obj.type == 'select-multiple' || obj.type == 'radio' || obj.type == 'checkbox' || obj.type == 'number') {
		// フォーカスを受け取った時（関数を追加）
		setEventFunction(obj, "onfocus", eventFocus);
		var className = obj.className;

		if (className != "") {
			if (className.indexOf("MAIL_ADDRESS") >= 0 ||
				className.indexOf("HALF_ENGLISH_NUMBER") >= 0 ||
				className.indexOf("PASSWORD") >= 0 ||
				className.indexOf("HALF_ENGLISH") >= 0) {
				// 何もしない
			} else if (className.indexOf("NUMBER") >= 0) {
				setEventFunction(obj, "onfocus", eventForcusNumber);
			}
		}

		// フォーカスを失った時
		setEventFunction(obj, "onblur", eventBlur);
		if (className != "") {
			if (className.indexOf("MAIL_ADDRESS") >= 0 ||
				className.indexOf("HALF_ENGLISH_NUMBER") >= 0 ||
				className.indexOf("PASSWORD") >= 0 ||
				className.indexOf("HALF_ENGLISH") >= 0) {
				setEventFunction(obj, "onblur", eventCheckFormat);
			} else if (className.indexOf("NUMBER") >= 0) {
				setEventFunction(obj, "onblur", eventFormatNumber);
			} else if (className.indexOf("DATE_YYMM") >= 0) {
				// YYYY/MM
				setEventFunction(obj, "onblur", eventFormatDateYYMM);
			} else if (className.indexOf("DATE_TIME") >= 0) {
				// YYYY/MM/DD hh:mi:ss
				setEventFunction(obj, "onblur", eventFormatDateTime);
			} else if (className.indexOf("DATE_HOUR_MINUTES") >= 0) {
				// YYYY/MM/DD hh:mi
				setEventFunction(obj, "onblur", eventFormatDateHourMinutes);
			} else if (className.indexOf("TIMESTAMP") >= 0) {
				// YYYY/MM/DD hh:mi:ss sss
				setEventFunction(obj, "onblur", eventFormatTimestamp);
			} else if (className.indexOf("DATE") >= 0) {
				// YYYY/MM/DD
				setEventFunction(obj, "onblur", eventFormatDate);
			} else if (className.indexOf("TIME") >= 0) {
				// hh:mi
				setEventFunction(obj, "onblur", eventFormatTime);
			} else if (className.indexOf("TEL_") >= 0 || className.indexOf("ZIP_") >= 0) {
				setEventFunction(obj, "onblur", eventTohalf);
			} else if (className.indexOf("MIXED") >= 0) {
				// エラークリア
//				setEventFunction(obj, "onblur", eventClearError);
			} else if (className.indexOf("REQUIRED_") >= 0) {
				// エラークリア
//				setEventFunction(obj, "onblur", eventClearError);
			}

		}
	}
}

function setEventFunction(obj, e, func) {
	if (obj.attachEvent != undefined) {
		obj.attachEvent(e, func);
	} else if (obj.addEventListener != undefined) {
		var ev = e.replace(/^on/, "");
		obj.addEventListener(ev, func, false);
	}
}

function removeEventListener(obj) {
	if (obj.type == 'text' || obj.type == 'textarea' ||  obj.type == 'password' || obj.type == 'select-one' || obj.type == 'select-multiple' || obj.type == 'radio' || obj.type == 'checkbox') {
		// フォーカスを受け取った時（関数を追加）
		removeEventFunction(obj, "onfocus", eventFocus);

		var className = obj.className;
		if(className != "") {
			if (className.indexOf("MAIL_ADDRESS") >= 0 ||
				className.indexOf("HALF_ENGLISH_NUMBER") >= 0 ||
				className.indexOf("PASSWORD") >= 0 ||
				className.indexOf("HALF_ENGLISH") >= 0) {
					// 何もしない
			} else if (className.indexOf("NUMBER") >= 0) {
				removeEventFunction(obj, "onfocus", eventForcusNumber);
			}
		}

		// フォーカスを失った時
		setEventFunction(obj, "onblur", eventBlur);
		if (className != "") {
			if (className.indexOf("MAIL_ADDRESS") >= 0 ||
				className.indexOf("HALF_ENGLISH_NUMBER") >= 0 ||
				className.indexOf("PASSWORD") >= 0 ||
				className.indexOf("HALF_ENGLISH") >= 0) {
				removeEventFunction(obj, "onblur", eventCheckFormat);
			} else if (className.indexOf("NUMBER") >= 0) {
				removeEventFunction(obj, "onblur", eventFormatNumber);
			} else if(className.indexOf("DATE_YYMM") >= 0) {
				// YYYY/MM
				removeEventFunction(obj, "onblur", eventFormatDateYYMM);
			} else if (className.indexOf("DATE_TIME") >= 0) {
				// YYYY/MM/DD hh:mi:ss
				removeEventFunction(obj, "onblur", eventFormatDateTime);
			} else if (className.indexOf("DATE_HOUR_MINUTES") >= 0) {
				// YYYY/MM/DD hh:mi:ss
				removeEventFunction(obj, "onblur", eventFormatDateHourMinutes);
			} else if (className.indexOf("DATE") >= 0) {
				// YYYY/MM/DD
				removeEventFunction(obj, "onblur", eventFormatDate);
			} else if (className.indexOf("TIMESTAMP") >= 0) {
				// YYYY/MM/DD hh:mi:ss sss
				removeEventFunction(obj, "onblur", eventFormatTimestamp);
			} else if (className.indexOf("TIME") >= 0) {
				// hh:mi
				removeEventFunction(obj, "onblur", eventFormatTime);
			} else if (className.indexOf("TEL_") >= 0 || className.indexOf("ZIP_") >= 0) {
				removeEventFunction(obj, "onblur", eventTohalf);
			} else if (className.indexOf("MIXED") >= 0) {
				// エラークリア
//				removeEventFunction(obj, "onblur", eventClearError);
			} else if (className.indexOf("REQUIRED_") >= 0) {
				// エラークリア
//				removeEventFunction(obj, "onblur", eventClearError);
			}

		}
	}
}

function removeEventFunction(obj, e, func) {
	if (obj.detachEvent != undefined) {
		obj.detachEvent(e, func);
	}
	else if (obj.removeEventListener != undefined) {
		var ev = e.replace(/^on/, "");
		obj.removeEventListener(ev, func, false);
	}
}

function getEventObject(event) {
	return window.event?window.event.srcElement:event.target;
}

function eventFocus(e) {
	var obj = getEventObject(e);
	inCursor(obj);

	// フォーカスが入ったときの値を記憶し、ユーザー入力があるか判定可能にする
	saveFocusValue(obj);
}

function saveFocusValue(obj) {
	obj.inFocusValue = obj.value;
}

function eventBlur(e) {
	var obj = getEventObject(e);
	outCursor(obj);
	if (isNullorUndefined(obj)) {
		return;
	}

	var className = obj.className;
	if (className.indexOf("TEL_1") >= 0 || className.indexOf("TEL_2") >= 0 || className.indexOf("ZIP_1") >= 0) {
		if (obj.value != obj.inFocusValue) {
			// 複数領域を1つの値として扱う電話番号(3カラム)、郵便番号(2カラム)のような領域の場合、前の領域の変更有無を記憶
			// 最後のエリア入力時のAjaxなど実行判定で利用する
			obj.isMultiPreAreaChangeFocusOutValue = true;
		}
	}

	obj.inFocusValue = null;
}

/**
 * 前領域フォーカスアウト時に値の変更がされたかを返却します。
 * 
 * @param obj 対象エレメント
 * @returns (前領域フォーカスアウト時の)対象エレメントとしての変更があった場合は、true
 */
function isChangeMultiPreAreaChangeFocusOutValue(obj) {
	if (isNullorUndefined(obj)) {
		return false;
	}
	if (isNullorUndefined(obj.isMultiPreAreaChangeFocusOutValue)) {
		return false;
	}
	
	if (obj.isMultiPreAreaChangeFocusOutValue) {
		return true;
	}
	return false;
}

/**
 * 前領域フォーカスアウト時に値の変更がされたかの記憶をクリアします。
 * 
 * @param obj 対象エレメント
 */
function clearMultiAreaChangeFocusOutValue(obj) {
	if (!isNullorUndefined(obj)) {
		obj.isMultiPreAreaChangeFocusOutValue = null;
	}
}

/**
 * 対象のエレメントの値にユーザー入力があったか判定します。
 * @param obj 対象エレメント
 * @returns ユーザー入力があった場合true
 */
function isChangeObject(obj) {
	if (obj.isChangeScriptLock) {
		return false;
	}
	
	if (!isNullorUndefined(obj.isChangeByScript) && obj.isChangeByScript) {
		obj.isChangeByScript = false;
		return true;
	}
	if (obj.inFocusValue != obj.value) {
		return true;
	} else {
		return false;
	}
}

/**
 * 画面にユーザー入力があったかを判定する
 * @return ユーザー入力があった場合true
 */
function isInputScreenValue() {
	var objForms = document.forms;
	for(var i = 0; i < objForms.length; i++) {
		for(var j = 0; j < objForms[i].elements.length; j++) {
			var obj = objForms[i].elements[j];
			if (isObjectInputType(obj)) {
				var initValue = obj.initValue;
				var nowValue = obj.value;
				if (initValue != nowValue) {
					return true;
				}
			}	
		}
	}
	return false;
}

function eventForcusNumber(e) {
	var obj = getEventObject(e);
	var number = obj.value;
	if(number == null || number == "") {
		return;
	} else {
		// カンマを外す
		obj.value = removeCanma(number);
		return;
	}
}

function removeCanma(number) {
	if (number == null || number == undefined) {
		return "";
	} else {
		return number.replace(/,/g, "");
	}
}

function eventTohalf(e) {
	var obj = getEventObject(e);
//	Validator.init(obj);
	if(obj.value == null || obj.value == "") {
		return;
	}
	
	var half = Validator.tohalf(obj.value);
	obj.value = half;
}

function eventFormatNumber(e) {
	var obj = getEventObject(e);
//	Validator.init(obj);
	if(obj.value == null || obj.value == "") {
		return;
	}

	var className = obj.className;
	var isSeparate = true;
	if (className.indexOf("NOT_SEPARATED") >= 0) {
		isSeparate = false;
	} else {
		isSeparate = true;
	}

	var periodNumber = 0;
	if (className.indexOf("PERIOD_") >= 0) {
		var ns = className.split("PERIOD_");
		if (ns.length > 1) {
			periodNumber = Number(ns[1].split(" ")[0]);
		}
	} else {
		periodNumber = 0;
	}
	
	try {
		var half = Validator.tohalf(obj.value);
		obj.value = half;
		var formatNumber = getFormatedNumberString(obj.value, periodNumber, isSeparate, true);
		if (className.indexOf("NUMBER_STRING") < 0) {
			// データ型が文字列の数値は、数値チェックはするが、フォーマット変換しない
			obj.value = formatNumber;
		}
	} catch(e) {
		if(e instanceof ExNumberException) {
			Validator.output(obj, e.message);
		} else {
			throw e;
		}
	}	
}

function eventFormatDate(e) {
	var obj = getEventObject(e);
	setFormatDate(obj);
}

function eventFormatDateYYMM(e) {
	var obj = getEventObject(e);
	setFormatDateYYMM(obj);
}

function eventFormatDateTime(e) {
	var obj = getEventObject(e);
	setFormatDateTime(obj);
}

function eventFormatDateHourMinutes(e) {
	var obj = getEventObject(e);
	setFormatDateHourMinutes(obj);
}

function eventFormatTimestamp(e) {
	var obj = getEventObject(e);
	setFormatTimestamp(obj);
}

function eventFormatTime(e) {
	var obj = getEventObject(e);
	setFormatTime(obj);
}

function eventClearError(e) {
	var obj = getEventObject(e);
	Validator.init(obj);
}

function eventCheckFormat(e) {
	var obj = getEventObject(e);
	var className = obj.className;
    if (className.indexOf("MAIL_ADDRESS") >= 0) {
//		Validator.init(obj);
    	Validator.check(obj, "mail", null, false);
    } else if (className.indexOf("HALF_ENGLISH_NUMBER") >= 0) {
//		Validator.init(obj);
    	Validator.check(obj, "alphabetNumber", null, false);
    } else if (className.indexOf("PASSWORD") >= 0) {
//		Validator.init(obj);
    	Validator.check(obj, "password", null, false);
    } else if (className.indexOf("HALF_ENGLISH") >= 0) {
//		Validator.init(obj);
    	Validator.check(obj, "alphabet", null, false);
    }
}

function eventCheckRequired(e) {
	var obj = getEventObject(e);
//	Validator.init(obj);
	Validator.check(obj, "input", null, false);
}

// FunctionKey Trap
// idがFNxxをClickして実現します。例えば、<input type="hidden" id="F12" onClick="submit()">を
// form中にコーディングしておくと、onClickを、F12で実行できる
// または、	<button name="F12" type="submit" class="Button65" id="F12" tabindex="901">ＯＫ</button>
function eventKeyDown(e){
	var obj;
	var objForm;
	e = e || window.event;
	var keyCode = getKeyCode(e);

	if (keyCode > 112 && keyCode < 124) {
		objForm = document.forms;
		var isCtrlKeyPush = e.ctrlKey;
		for(var i = 0; i < objForm.length; i++) {
			for(var n = 0; n < objForm[i].elements.length; n++) {
				obj = objForm[i].elements[n];
				for(var p = 0; p < fnKeyName.length; p++) {
					var targetFnkeyName = fnKeyName[p];
					if (isCtrlKeyPush) {
						targetFnkeyName = "Ctrl" + fnKeyName[p];
					}
					if (obj.id == targetFnkeyName && keyCode == fnKeyCode[p]) {
						if (obj.type != 'hidden') {
							// hiddenにfocusを送るとエラーとなるため、hidden項目の場合は除外
							obj.focus();
						}
						if (!obj.disabled) {
							obj.click();
						}
						setKeyCode(e, KEY_NONE);
						return false;
					}
				}
   			}
  		}
	}
}

function getKeyCode(e){
    if (document.all) {
    	if (!isNullorUndefined(event)) {
        	return  event.keyCode;
        }
    }
    else if (document.getElementById) {
        return (e.keyCode)? e.keyCode: e.charCode;
    }
    else if (document.layers) {
        return  e.which;
    }
}

function setKeyCode(e, value){
    if (document.all) {
    	if (!isNullorUndefined(event)) {
	        event.keyCode = value;
	    }
    }
    else if (e.preventDefault) {
    	e.preventDefault();
    	e.stopPropagation();
    }
    else if (document.layers) {
        e.which = value;
    }
}

// EnterKey Trap
function eventEntry(e){
	var obj = this;

	keyCode = getKeyCode(e);
	if (keyCode == KEY_BS){	//redaonlyのBackSpaceで「戻る」を防ぐ
		var evObj = getEventObject(e);
		if (evObj.type == "text") {
 			if (evObj.readOnly) {
				setKeyCode(e, KEY_NONE);
				return false;
			}
		}
	}
	return true;
}

// 入力フィールド全てをReadOnly
function readOnlyBody(){
	var n;
	var i;
	var obj;
	var objForm;

	objForm = document.forms;
	for(i = 0; i < objForm.length; i++){
		for(n = 0; n < objForm[i].elements.length; n++){
			obj = objForm[i].elements[n];
			if(obj.type == 'text' || obj.type == 'radio' || obj.type == 'checkbox' || obj.type == 'select-one' || obj.type == 'select-multiple' || obj.type == 'textarea' ||  obj.type == 'password' || obj.type == 'button' || obj.type == 'submit' || obj.type == 'reset'){
				obj.readOnly=true;
				if(obj.type == 'text' || obj.type == 'select-one' || obj.type == 'select-multiple' || obj.type == 'textarea' ||  obj.type == 'password' || obj.type == 'button' || obj.type == 'submit' || obj.type == 'reset'){
					obj.style.backgroundColor = "#F6F4EA";
				}
			}
		}
	}
}

/*********************************************************************************************************************************************/
/**  FireFox用　テーブルスクロール
/*********************************************************************************************************************************************/
function makeFixHeader(obj) {
	// ヘッダーとなる行オブジェクトを取得
	var thead = getFirstElementByTagName(obj, "THEAD");
	// ヘッダーが見つからなければ処理しない
	if (thead == null) return;

	// スクロール設定するTABLEを取得（THEADの親ノードは必ずTABLE）
	var table = thead.parentNode;

	// 新しくヘッダーとなるTABLEを作成し、各属性をコピー
	var newTable = document.createElement("TABLE");
	newTable.width = table.offsetWidth;
	newTable.className = table.className;
	newTable.setAttribute("style", table.getAttribute("style"));
	newTable.style.marginBottom = "0px";

	// スクロールバーの幅
	// センタリングした場合、スクロールバーの分だけ左にずれるので補正
	var scrollBar = obj.offsetWidth - table.offsetWidth;
	newTable.style.marginRight = scrollBar + "px";

	var newHead = thead.cloneNode(true);
	newTable.appendChild(newHead);

	// Divの前に挿入
	obj.parentNode.insertBefore(newTable, obj);

	// THEADは非表示(ヘッダーのみにサイズ指定されている場合があるので、旧ヘッダーの非表示は必ず最後)
	var tr = getFirstElementByTagName(thead, "TR");
	if (tr != null) {
		// ヘッダー部を隠し、行の高さを０にする。（display:none;を使用するとセル幅が変わってしまう）
		tr.style.visibility = "hidden";
		for(var i=0 ; i<tr.cells.length ; i++) {
			var cell = tr.cells[i];
			cell.innerHTML = "";
			cell.style.padding = "0px";
		}
	}

}

function getFirstElementByTagName(obj, name) {
	var child;
	for(var i=0 ; i<obj.childNodes.length ; i++) {
		child = obj.childNodes[i];
		if (child.tagName == name) {
			// 最初に見つかった
			return child;
		}
		else if (child.hasChildNodes()) {
			var find = getFirstElementByTagName(child, name);
			if (find != null) return find;
		}
	}
	return null;
}

function fixColWidth(obj, arr, current) {
	if (obj == null) return;
	current = (current==null)?0:current;

	// セットした幅の配列
	var result = new Array();

	// セル幅を設定
	var child;
	var width;
	for(var i=0 ; i<obj.childNodes.length ; i++) {
		child = obj.childNodes[i];
		if (child.tagName == "TD" || child.tagName == 'TH') {
			if (arr != null && arr.length > current) {
				width = arr[current];
			}
			else {
				// 設定する値がなければ現在幅
				width = child.offsetWidth;
			}
			child.style.width = width + "px";
			child.width = width;

			result[result.length] = width;
			current ++;
		}
		else if (child.hasChildNodes()) {
			var  ret = fixColWidth(child, arr, current);
			for(var j=0 ; j<ret.length ; j++) result[result.length] = ret[j];
		}
	}

	return result;
}

/*********************************************************************************************************************************************/
/**  入力フォーカス
/*********************************************************************************************************************************************/
function inCursor(obj){
	obj.className = removeStyleClass(obj.className) + " focus";
}

function outCursor(obj){
	obj.className = removeStyleClass(obj.className);
}

function removeStyleClass(className) {
	var ac = className.split(" ");
	className = ac[0];
	for (var i = 1; i < ac.length; i++) {
		if (ac[i] != "focus") {
			className += " " + ac[i];
		}
	}
	return className;
}

/*********************************************************************************************************************************************/
/**  Exception: 例外クラス
/*********************************************************************************************************************************************/
//
// 数字例外
//  数字以外の文字を検証した場合にスローする
//
var ExNumberException = function() {
	this.initialize.apply(this, arguments);
}
ExNumberException.prototype = {
	message: "",
	name: "ExNumberException",

	initialize: function(message) {
		this.message = message;
	},

	toString: function () {
		return this.name + ': "' + this.message + '"';
	}
};

//
// 日付例外
//  認識できない日付書式や、日付の妥当性の検証でスローする
//
var ExDateTimeException = function() {
	this.initialize.apply(this, arguments);
}
ExDateTimeException.prototype = {
	message: "",
	name: "ExDateTimeException",

	initialize: function(message) {
		this.message = message;
	},

	toString: function () {
		return this.name + ': "' + this.message + '"';
	}
};

/*********************************************************************************************************************************************/
/**  ExNumber: 数字クラス
/*********************************************************************************************************************************************/
var ExNumber = function() {
	this.initialize.apply(this, arguments);
}
ExNumber.prototype = {
	// インスタンス変数
	num: "",
	minus: false,

	//
	// コンストラクタ
	//  str: 数字文字列
	//
	initialize: function(str) {
		if (str != null) {
			str = str.toString();
			str = trim(str);
			str = removeCanma(str);						// カンマを外す
			str = this.zeroSuppress(str);				// ゼロサプレス

			// 数値チェック
			for(var i=0 ; i<str.length ; i++) {
				var c = str.substring(i, i+1);
				if (c != "-" && c != "." && (c < "0" || c > "9")) {
					throw new ExNumberException("数値を入力してください。");
				}
			}

			// 途中にあるマイナスを除去（123-456 → 12345)
			str = str.replace(/([0-9.]+)\-+([0-9.]*?)/g, "$1$2");

			// 複数ある"."を最後を残し除去（123.45.6 → 12345.6)
			var idx = str.lastIndexOf(".");
			if (idx >= 0) {
				var n = str.substring(0, idx);
				n = n.replace(/([0-9]*?)\.+([0-9]*?)/g, "$1$2");
				str = n + str.substring(idx);
				if (str.substring(idx).length <= 1) {
					str += "0";
				}
			}
			if (str.replace(/[-0.]/g, "") == "") str = "0";
			this.num = str;

			if (this.num.substring(0, 1) == "-") this.minus = true;
		}
	},

	//
	// ゼロサプレス
	//  String num: 前ゼロサプレスする数字文字列
	//  return: 前ゼロサプレスした結果を返す
	//
	zeroSuppress: function(num) {
		return num.replace(/^(-*?)0+([0-9]+.*)/, "$1$2");
	},

	//
	// 指定数字文字列のマイナス検証
	//  String num: 前ゼロサプレスする数字文字列
	//  return: 負の場合はtrue、正の場合はfalseを返す
	//
	isMinus: function() {
		return this.minus;
	},

	//
	// 実数（小数点あり）検証
	//  return: 実数の場合はtrue、整数の場合はfalseを返す
	//
	isFloat: function() {
		return (this.num.indexOf(".") >= 0)?true:false;
	},

	//
	// 絶対値の取得
	//  return: 絶対値を返す
	//
	abs: function() {
		return this.num.replace(/-/g, "");
	},

	//
	// Numberクラスに変換
	//  return: Numberクラスを返す
	//
	toNumber: function() {
		return new Number(this.num);
	},

	//
	// 数字文字列の取得
	//  return: 数字文字列を書式化せずそのまま返す
	//
	toString: function() {
		return this.num;
	},

	//
	// 数字文字列の書式化取得
	// 	fmt: 書式
	//       書式は"#,##0.0" か "000000"で指定可能（Default:"#,##0.0"）
	//  return: 書式化した数字文字列を返す
	//
	toFormatString: function(fmt) {
		var s = this.num;

		// サイン部を消し、整数部と小数部に分ける
		s = s.replace(/-/g, "");
		var idx = s.indexOf(".");
		var f = null;
		if (idx >= 0) {
			f = s.substring(idx+1);				// 小数
			s = s.substring(0, idx);				// 整数
		}
		if (s == "0" && fmt.indexOf("#.") >= 0) s = "";
		if (s == "" && fmt.indexOf("0.") >= 0) s = "0";

		// 書式
		if (fmt == null || fmt == "") {
			fmt = "#,##0";
		}
		else {
			// 前後空白は無視
			fmt = trim(fmt);
		}

		// 書式チェック("-#,##0.00" or "0000000")
		if (fmt.replace(/0/g, "") == "") {
			// 書式00000の場合は小数部は処理しない
			var len = fmt.length;
			if (s.length >= len) {
				return s;
			}
			else {
				var zero = "";
				for(var i=0 ; i<len-s.length ; i++) {
					zero += "0";
				}
				return zero+s;
			}
		}
		else if (fmt.replace(/[-#0.,]/g, "") == "") {	// 書式は厳密なチェックはしない。指定文字があればOK
			var n = "";
			var canma = "";
			if (fmt.indexOf(",") >= 0) canma = ",";

			// 整数部
			var len = s.length;
			for(var i=0 ; i<len ; i++) {
				if (i > 0 && (i%3) == 0) {
					n = canma + n;
				}
				n = s.substring(len-i-1, len-i) + n;
			}
			// 小数部（指定がある場合。指定がない場合は処理しない）
			idx = fmt.indexOf(".");
			if (idx >= 0) {
				n += ".";
				len = fmt.substring(idx).length-1;
				if (f == null) {
					for(var i=0 ; i<len ; i++) n += "0";
				}
				else {
					if (f.length >= len) {
						n += f.substring(0, len);
					}
					else {
						n += f;
						for(var i=0 ; i<len-f.length ; i++) n += "0";
					}
				}
			}
			if (this.isMinus() && n.replace(/[0.]/g, "") != "") n = "-" + n;
			return n;
		}

		// 該当しない書式では変換しない
		return this.num;
	}
};

/*********************************************************************************************************************************************/
/**  ExDateTime: 日付／時間クラス
/*********************************************************************************************************************************************/
var ExDateTime = function() {
	this.initialize.apply(this, arguments);
}
ExDateTime.prototype = {
	date: null,

	//
	// コンストラクタ
	//
	initialize: function() {
		this.date = new Date();
	},

	//
	// 日時を取得
	//  return: 日時を返す
	//
	get: function() {
		return this.date;
	},

	//
	// 日時を設定
	//  String(int) year  : 年（省略(null)した場合のDefaultは当年）
	//  String(int) month : 月（1～12の範囲。省略(null)した場合のDefaultは当月）
	//  String(int) day   : 日（1～31の範囲。省略(null)した場合のDefaultは当日）
	//  String(int) hour  : 時（0～23の範囲。省略(null)した場合のDefaultは0）
	//  String(int) minute: 分（0～59の範囲。省略(null)した場合のDefaultは0）
	//  String(int) second: 秒（0～59の範囲。省略(null)した場合のDefaultは0）
	//
	// 変換に失敗した場合はExceptionをスローする。
	//  ExDateTimeException: 日付の誤り
	//  ExNumberException  : 日付数字部分に文字
	//
	set: function(year, month, day, hour, minute, second, millisec) {
		var yy = this.toNumber(year, this.date.getFullYear());
		if (yy < 100) yy += 2000;						// ２桁入力の場合は+2000（年）
		var mm = this.toNumber(month, this.date.getMonth() + 1);
		var dd = this.toNumber(day, this.date.getDate());
		var hh = this.toNumber(hour, 0);
		var mi = this.toNumber(minute, 0);
		var ss = this.toNumber(second, 0);
		var sss = this.toNumber(millisec, 0);

		// For Debug
		// alert(yy + "," + mm + "," + dd + "," + hh + "," + mi + "," + ss + "," + sss);

		// 日付の妥当性チェック
		if (!this.checkDate(yy, mm, dd, hh, mi, ss, sss)) {
			throw new ExDateTimeException("正しい日付を入力してください。");
		}

		this.date = new Date(yy, mm-1, dd, hh, mi, ss, sss);
	},

	//
	// 文字を数字に変換
	//  String strNum : 変換する数字文字列
	//  int def       : strNumがnullの場合のDefault値
	//  return : 変換した数字。
	//
	// 変換に失敗した場合はExceptionをスローする。
	//  ExNumberException  : 日付数字部分に文字
	//
	toNumber: function(strNum, def) {
		var num = 0;
		if (strNum == null || strNum == "") {
			return def;
		} else if (typeof strNum == "number") {
			return strNum;
		} else {
			num = parseInt(strNum, 10);
			if (isNaN(num)) {
				throw new ExNumberException("数字を入力してください。");
			}
		}
		return num;
	},

	//
	// 日付の妥当性チェック
	//  int year   : 年
	//  int month  : 月
	//  int day    : 日
	//  int hour   : 時
	//  int minute : 分
	//  int second : 秒
	//  int msec   : ミリ秒
	//  return : 正常の場合はtrue、エラーがある場合はfalseを返す。
	//
	checkDate: function(year, month, day, hour, minute, second, msec) {
		// 月
		if (month < 1 || month > 12) return false;

		// 日（閏年チェック含む）
		var lastDate = ExDateTimeUtils.getLastDayOfMonth(year, month);
		if (day < 1 || day > lastDate) return false;

		// 時
		if (hour < 0 || hour > 23) return false;

		// 分
		if (minute < 0 || minute > 59) return false;

		// 秒
		if (second < 0 || second > 59) return false;

		// ミリ秒
		if (msec < 0 || msec > 999) return false;

		return true;
	},

	//
	// 年月(YYYY/MM)形式の日付を解析
	//  String str : 解析する日付文字列
	//  return : 正常の場合は解析した日付を持つ自インスタンスを返す。
	//
	// 解析に失敗した場合はExceptionをスローする。
	//  ExDateTimeException: 日付の入力形式誤り
	//
	parseYM: function(str) {
		if (str.match(/^(\d{2})$/)) {								// mm
			this.set(null, RegExp.$1, 1, 0, 0, 0);
		} else if (str.match(/^(\d{4})$/)) {						// yymm
			this.set(str.substring(0, 2), str.substring(2), 1, 0, 0, 0);
		} else if (str.match(/^(\d{6})$/)) {						// yyyymm
			this.set(str.substring(0, 4), str.substring(4), 1, 0, 0, 0);
		} else if (str.match(/^(\d{2,4})\/{0,1}(\d{1,2})$/)) {		// yyyy/mm
			this.set(RegExp.$1, RegExp.$2, 1, 0, 0, 0);
		} else {
			throw new ExDateTimeException("yyyy/MM の形式で入力してください。");
		}
		return this;
	},

	//
	// 日時(YYYY/MM/DD hh:mi:ss)形式の日付を解析
	//  String str : 解析する日付文字列
	//  return : 正常の場合は解析した日付を持つ自インスタンスを返す。
	//
	// 解析に失敗した場合はExceptionをスローする。
	//  ExDateTimeException: 日付の入力形式誤り
	//
	parseDate: function(str, isTimeNeed) {
		if (str.match(/^(\d{2})$/)) {							// dd
			this.set(null, null, RegExp.$1, 0, 0, 0);
		} else if (str.match(/^(\d{4})$/)) {						// mmdd
			this.set(null, str.substring(0, 2), str.substring(2), 0, 0, 0);
		} else if (str.match(/^(\d{6})$/)) {						// yymmdd
			this.set(str.substring(0, 2), str.substring(2, 4), str.substring(4), 0, 0, 0);
		} else if (str.match(/^(\d{8})$/)) {						// yyyymmdd
			this.set(str.substring(0, 4), str.substring(4, 6), str.substring(6), 0, 0, 0);
		} else if (str.match(/^(\d{2,4})\/{0,1}(\d{1,2})\/{0,1}(\d{1,2})$/)) { // yyyy/mm/dd
			this.set(RegExp.$1, RegExp.$2, RegExp.$3, 0, 0, 0);
		} else if (isTimeNeed && str.match(/^(\d{2,4})\/{0,1}(\d{1,2})\/{0,1}(\d{1,2})\s*?(\d{0,2})\:{0,1}(\d{0,2})\:{0,1}(\d{0,2})$/)) { // yyyy/mm/dd hh:mi:ss
			this.set(RegExp.$1, RegExp.$2, RegExp.$3, RegExp.$4, RegExp.$5, RegExp.$6);
		} else {
			var message;
			if (isTimeNeed) {
				message = "yyyy/MM/dd HH:mm:ss の形式で入力してください。"
			} else {
				message = "yyyy/MM/dd の形式で入力してください。"
			}
			throw new ExDateTimeException(message);
		}
		return this;
	},

	//
	// 日時(YYYY/MM/DD hh:mi)形式の日付を解析
	//  String str : 解析する日付文字列
	//  return : 正常の場合は解析した日付を持つ自インスタンスを返す。
	//
	// 解析に失敗した場合はExceptionをスローする。
	//  ExDateTimeException: 日付の入力形式誤り
	//
	parseDateHourMinutes: function(str) {
		if (str.match(/^(\d{2})$/)) {							// dd
			this.set(null, null, RegExp.$1, 0, 0, 0);
		} else if (str.match(/^(\d{4})$/)) {						// mmdd
			this.set(null, str.substring(0, 2), str.substring(2), 0, 0, 0);
		} else if (str.match(/^(\d{6})$/)) {						// yymmdd
			this.set(str.substring(0, 2), str.substring(2, 4), str.substring(4), 0, 0, 0);
		} else if (str.match(/^(\d{8})$/)) {						// yyyymmdd
			this.set(str.substring(0, 4), str.substring(4, 6), str.substring(6), 0, 0, 0);
		} else if (str.match(/^(\d{2,4})\/{0,1}(\d{1,2})\/{0,1}(\d{1,2})$/)) { // yyyy/mm/dd
			this.set(RegExp.$1, RegExp.$2, RegExp.$3, 0, 0, 0);
		} else if (str.match(/^(\d{2,4})\/{0,1}(\d{1,2})\/{0,1}(\d{1,2})\s*?(\d{0,2})\:{0,1}(\d{0,2})$/)) { // yyyy/mm/dd hh:mi
			this.set(RegExp.$1, RegExp.$2, RegExp.$3, RegExp.$4, RegExp.$5, 0);
		} else {
			var message = "yyyy/MM/dd HH:mm の形式で入力してください。"
			throw new ExDateTimeException(message);
		}
		return this;
	},

	//
	// Timestamp(YYYY/MM/DD hh:mi:ss sss)形式の日付を解析
	//  String str : 解析する日付文字列
	//  return : 正常の場合は解析した日付を持つ自インスタンスを返す。
	//
	// 解析に失敗した場合はExceptionをスローする。
	//  ExDateTimeException: 日付の入力形式誤り
	//
	parseTimestamp: function(str, isTimeNeed) {
		if (str.match(/^(\d{2})$/)) {							// dd
			this.set(null, null, RegExp.$1, 0, 0, 0);
		} else if (str.match(/^(\d{4})$/)) {						// mmdd
			this.set(null, str.substring(0, 2), str.substring(2), 0, 0, 0);
		} else if (str.match(/^(\d{6})$/)) {						// yymmdd
			this.set(str.substring(0, 2), str.substring(2, 4), str.substring(4), 0, 0, 0);
		} else if (str.match(/^(\d{8})$/)) {						// yyyymmdd
			this.set(str.substring(0, 4), str.substring(4, 6), str.substring(6), 0, 0, 0);
		} else if (str.match(/^(\d{2,4})\/{0,1}(\d{1,2})\/{0,1}(\d{1,2})$/)) { // yyyy/mm/dd
			this.set(RegExp.$1, RegExp.$2, RegExp.$3, 0, 0, 0);
		} else if (isTimeNeed && str.match(/^(\d{2,4})\/{0,1}(\d{1,2})\/{0,1}(\d{1,2})\s*?(\d{0,2})\:{0,1}(\d{0,2})\:{0,1}(\d{0,2})$/)) { // yyyy/mm/dd hh:mi:ss
			this.set(RegExp.$1, RegExp.$2, RegExp.$3, RegExp.$4, RegExp.$5, RegExp.$6);
		} else if (isTimeNeed && str.match(/^(\d{2,4})\/{0,1}(\d{1,2})\/{0,1}(\d{1,2})\s*?(\d{0,2})\:{0,1}(\d{0,2})\:{0,1}(\d{0,2})\s*?(\d{0,3})$/)) { // yyyy/mm/dd hh:mi:ss sss
			this.set(RegExp.$1, RegExp.$2, RegExp.$3, RegExp.$4, RegExp.$5, RegExp.$6, RegExp.$7);
		} else {
			var message;
			if (isTimeNeed) {
				message = "yyyy/MM/dd HH:mm:ss sss の形式で入力してください。"
			} else {
				message = "yyyy/MM/dd の形式で入力してください。"
			}
			throw new ExDateTimeException(message);
		}
		return this;
	},

	//
	// 時間(hh:mi:ss)形式の日付を解析
	//  String str : 解析する時間文字列
	//  return : 正常の場合は解析した時間を持つ自インスタンスを返す。
	//
	// 解析に失敗した場合はExceptionをスローする。
	//  ExDateTimeException: 時間の入力形式誤り
	//
	parseTime: function(str) {
		if (str.match(/^(\d{1,2})$/)) {							// hh
			this.set(null, null, null, RegExp.$1, 0, 0);
		} else if (str.match(/^(\d{4})$/)) {						// hhmm
			this.set(null, null, null, str.substring(0, 2), str.substring(2, 4), 0);
		} else if (str.match(/^(\d{6})$/)) {						// hhmmss
			this.set(null, null, null, str.substring(0, 2), str.substring(2, 4), str.substring(4));
		} else if (str.match(/^(\d{1,2})\:(\d{1,2})$/)) {			// hh:mm
			try {
				this.set(null, null, null, RegExp.$1, RegExp.$2, 0);
			} catch (e) {
				e.message = "正しい時刻を入力してください。";
				throw e;
			}
		} else if (str.match(/^(\d{1,2})\:(\d{1,2})\:(\d{1,2})$/)) {	// hh:mm:ss
			try {
				this.set(null, null, null, RegExp.$1, RegExp.$2, RegExp.$3);
			} catch (e) {
				e.message = "正しい時刻を入力してください。";
				throw e;
			}
		} else {
			throw new ExDateTimeException("hh:mm の形式で入力してください。");
		}
		return this;
	},

	//
	// 年の加算
	//  String(int) num : 加算する数値（マイナスの場合は減算）
	//  return : 加算後の自インスタンスを返す。
	//
	// 数字以外の文字を指定した場合はExceptionをスローする
	//  ExNumberException: 数字以外の文字
	//
	addYears: function(num) {
		num = this.toNumber(num, 0);
		this.date.setFullYear(this.date.getFullYear() + num);
		return this;
	},

	//
	// 月の加算
	//  String(int) num : 加算する数値（マイナスの場合は減算）
	//  return : 加算後の自インスタンスを返す。
	//
	// 数字以外の文字を指定した場合はExceptionをスローする
	//  ExNumberException: 数字以外の文字
	//
	addMonths: function(num) {
		num = this.toNumber(num, 0);
		this.date.setMonth(this.date.getMonth() + num);
		return this;
	},

	//
	// 日の加算
	//  String(int) num : 加算する数値（マイナスの場合は減算）
	//  return : 加算後の自インスタンスを返す。
	//
	// 数字以外の文字を指定した場合はExceptionをスローする
	//  ExNumberException: 数字以外の文字
	//
	addDays: function(num) {
		num = this.toNumber(num, 0);
		this.date.setDate(this.date.getDate() + num);
		return this;
	},

	//
	// 時の加算
	//  String(int) num : 加算する数値（マイナスの場合は減算）
	//  return : 加算後の自インスタンスを返す。
	//
	// 数字以外の文字を指定した場合はExceptionをスローする
	//  ExNumberException: 数字以外の文字
	//
	addHours: function(num) {
		num = this.toNumber(num, 0);
		this.date.setHours(this.date.getHours() + num);
		return this;
	},

	//
	// 分の加算
	//  String(int) num : 加算する数値（マイナスの場合は減算）
	//  return : 加算後の自インスタンスを返す。
	//
	// 数字以外の文字を指定した場合はExceptionをスローする
	//  ExNumberException: 数字以外の文字
	//
	addMinutes: function(num) {
		num = this.toNumber(num, 0);
		this.date.setMinutes(this.date.getMinutes() + num);
		return this;
	},

	//
	// 秒の加算
	//  String(int) num : 加算する数値（マイナスの場合は減算）
	//  return : 加算後の自インスタンスを返す。
	//
	// 数字以外の文字を指定した場合はExceptionをスローする
	//  ExNumberException: 数字以外の文字
	//
	addSeconds: function(num) {
		num = this.toNumber(num, 0);
		this.date.setSeconds(this.date.getSeconds() + num);
		return this;
	},

	//
	// 経過日数を取得
	//  Date(ExDateTime) date : 経過日数を計算する日付
	//  return 経過日数を返す。
	//
	progressDays: function(date) {
		var d1 = this.date.getTime();
		var d2;
		if (date instanceof ExDateTime) d2 = date.get().getTime();
		else if (date instanceof Date) d2 = date.getTime();
		else return null;

		var d = d2 - d1;
		return Math.floor(d / (24*60*60*1000));
	},

	//
	// 経過時間を取得
	//  Date(ExDateTime) date : 経過時間を計算する日付
	//  return 経過時間数を返す。
	//
	progressTimes: function(date) {
		var d1 = this.date.getTime();
		var d2;
		if (date instanceof ExDateTime) d2 = date.get().getTime();
		else if (date instanceof Date) d2 = date.getTime();
		else return null;

		var d = d2 - d1;
		return Math.floor(d / (60*60*1000));
	},

	//
	// 日付の比較
	//  Date(ExDateTime) date : 比較する日付
	//  return 比較する日付が大きい場合は負の数、同じ場合は０、小さい場合は正の数を返す。
	//
	compare: function(date) {
		var d1 = this.date.getTime();
		var d2;
		if (date instanceof ExDateTime) d2 = date.get().getTime();
		else if (date instanceof Date) d2 = date.getTime();
		else return null;

		if (d1 == d2) return 0;
		else if (d1 < d2) return -1;
		else return 1;
	},

	//
	// 日時の文字列取得
	//  return : 日時の文字列を返す。（書式はDate#toString()に依存する）
	//
	toString: function() {
		return this.date.toString();
	}
};

/*********************************************************************************************************************************************/
/**  ExDateTimeUtils: 日付／時間ユーティリティクラス
/*********************************************************************************************************************************************/
var ExDateTimeUtils = {

	//
	// 日付の書式化
	//  Date date  : 書式化する日付（Dateクラス）
	//  String fmt : 書式
	//   書式：
	//     # : Escape文字。連続する直後の書式文字をエスケープする
	//     Y : ４桁の西暦年
	//     y : 下２桁の西暦年
	//     m : 月（２桁）１桁の場合は前に0を付ける
	//     n : 月（１～２桁）前0は付けない
	//     d : 日（２桁）１桁の場合は前に0を付ける
	//     j : 日（１～２桁）前0は付けない
	//     w : 曜日（0:日曜日 ... 6:土曜日）
	//     a : 午前／午後の略号小文字（am/pm）
	//     A : 午前／午後の略号大文字（AM/PM）
	//     H : 時（24時間制 ２桁）１桁の場合は前に0を付ける
	//     h : 時（12時間制 ２桁）１桁の場合は前に0を付ける
	//     G : 時（24時間制 １～２桁）前0は付けない
	//     g : 時（12時間制 １～２桁）前0は付けない
	//     i : 分（２桁）１桁の場合は前に0を付ける
	//     s : 秒（２桁）１桁の場合は前に0を付ける
	//     f : ミリ秒（３桁）２桁以下の場合は前に0を付ける
	//     t : 指定月の最終日
	//     L : うるう年（うるう年の場合は"true"、以外の場合は"false"）
	//     z : 指定日の同年1月1日から数えた経過日数
	//     以外: 変換せず
	//
	//  return: 指定書式で変換した日付文字列を返す。
	//
	//   例） "Y/m/d H:i:s" → "2008/10/10 18:00:00"
	//
	format: function(date, format) {
	    if (typeof format != "string") return;

	    var d = date;
	    if (date instanceof ExDateTime) d = date.get();

		var dYear = d.getFullYear();
		var dMonth = d.getMonth() + 1;
		var dDate = d.getDate();
		var dDay = d.getDay();
		var dHours = d.getHours();
		var dMinutes = d.getMinutes();
		var dSeconds = d.getSeconds();
		var dMsec = d.getMilliseconds();
		var res = "";

		for(var i=0, len=format.length ; i<len ; i++) {
			var c = format.charAt(i);
			switch(c) {
				case "#":
					if (i == len - 1) break;
					res += format.charAt(++i);
					break;
				case "Y": res += dYear; break;
				case "y": res += dYear.toString().substr(2, 2); break;
				case "m": res += this.preZero(dMonth); break;
				case "n": res += dMonth; break;
				case "d": res += this.preZero(dDate); break;
				case "j": res += dDate; break;
				case "w": res += dDay; break;
				case "a": res += this.ampm(dHours); break;
				case "A": res += this.ampm(dHours).toUpperCase(); break;
				case "H": res += this.preZero(dHours); break;
				case "h": res += this.preZero(this.from24to12(dHours)); break;
				case "G": res += dHours; break;
				case "g": res += this.from24to12(dHours); break;
				case "i": res += this.preZero(dMinutes); break;
				case "s": res += this.preZero(dSeconds); break;
				case "t": res += this.getLastDayOfMonth(d); break;
				case "L": res += this.isLeapYear(dYear); break;
				case "z": res += this.getDayOfYear(dYear, dMonth, dDate); break;
				case "f": res += this.preWZero(dMsec); break;
				default : res += c; break;
			}
		}
		return res;
	},

	//
	// 前ゼロ処理
	//  int value : 前ゼロ編集する数字
	//  return : 前ゼロ編集した数字文字列を返す。
	//
	preZero: function(value) {
		return (parseInt(value) < 10) ? "0" + value : value;
	},

	//
	// 前ゼロ処理
	//  int value : 前ゼロ編集する数字
	//  return : 前ゼロ編集した数字文字列を返す。
	//
	preWZero: function(value) {
		var intVal = parseInt(value)
		if (intVal < 10) {
			return "00" + value;
		} else if (intVal < 100) {
			return "0" + value;
		} else {
			return value;
		}
	},

	//
	// 24時間制の時間を12時間制に変換
	//  int hours : 変換する時間
	//  return : 12時間制に変換した時間文字列
	//
	from24to12: function(hours) {
		return (hours > 12) ? hours - 12 : hours;
	},

	//
	// 午前／午後略号の取得（am/pm）
	//  int hours : 略号を取得する時間
	//  return 午前の場合はam、午後の場合はpmを返す。
	//
	ampm: function(hours) {
		return (hours < 12) ? "am" : "pm";
	},

	//
	// 指定月の最終日を取得
	//  int(Date) year : 年
	//  int month: 月（年にDateクラスが指定された場合は無視される）
	//  return 指定月の最終日を返す。
	getLastDayOfMonth: function(year, month) {
		var yy = year;
		var mm = month;
		if (year instanceof Date) {
			yy = year.getFullYear();
			mm = year.getMonth() + 1;
		}
		var tmp = new Date(yy, mm-1+1, 0);		// -1+1は、JavaScript Dateクラスで月は0～11のため指定月から-1をし、
													// 最終日を取得するため、翌月1日の前日とするため+1をする。
		return tmp.getDate();
	},

	//
	// 指定年のうるう年検証
	//  int year : 年
	//  return : うるう年の場合はtrue、以外の場合はfalseを返す。
	//
	isLeapYear: function(year) {
		var sum = this.getDayOfYear(year, 12, 31);
		return (sum == 365) ? false : true;
	},

	//
	// 指定日の経過日数を取得
	//  int year  : 年
	//  int month : 月
	//  int day   : 日
	//  return : 指定日の同年1月1日から指定日までの経過日数を返す。
	//
	getDayOfYear: function(year, month, day) {
		var sum = 0;
		for(var mm=1 ; mm<month ; mm++) {
			sum += this.getLastDayOfMonth(year, mm);
		}
		return sum + day;
	}
};

function trim(str) {
	if (str == null) return null;
	return str.replace(/(^\s*)|(\s*$)/g, "");
}

/*********************************************************************************************************************************************/
/**  日付補完: Form
/*********************************************************************************************************************************************/

// 日付：YYYYMMDD形式
function setFormatDate(obj) {
//	Validator.init(obj);
	if (obj.value == null || obj.value == "") return;
	try {
		obj.value = Validator.tohalf(obj.value);
		var date = new ExDateTime().parseDate(obj.value, false);
		obj.value = ExDateTimeUtils.format(date, "Y/m/d");
	}
	catch(e) {
		if (e instanceof ExDateTimeException) {
			Validator.output(obj, e.message);
		}
		else {
			throw e;
		}
	}
}

// 日付：YYMM形式
function setFormatDateYYMM(obj) {
//	Validator.init(obj);
	if (obj.value == null || obj.value == "") {
		return;
	}
	
	try {
		obj.value = Validator.tohalf(obj.value);
		var date = new ExDateTime().parseYM(obj.value);
		obj.value = ExDateTimeUtils.format(date, "Y/m");
	}
	catch(e) {
		if (e instanceof ExDateTimeException) {
			Validator.output(obj, e.message);
		}
		else {
			throw e;
		}
	}
}

//日時： YYYYMMDD hhmiss
function setFormatDateTime(obj) {
//	Validator.init(obj);
	if (obj.value == null || obj.value == "") {
		return;
	}

	try {
		obj.value = Validator.tohalf(obj.value);
		var date = new ExDateTime().parseDate(obj.value, true);
		obj.value = ExDateTimeUtils.format(date, "Y/m/d H:i:s");
	}
	catch(e) {
		if (e instanceof ExDateTimeException) {
			Validator.output(obj, e.message);
		}
		else {
			throw e;
		}
	}
}

//日時： YYYYMMDD hhmi
function setFormatDateHourMinutes(obj) {
//	Validator.init(obj);
	if (obj.value == null || obj.value == "") {
		return;
	}

	try {
		obj.value = Validator.tohalf(obj.value);
		var date = new ExDateTime().parseDateHourMinutes(obj.value);
		obj.value = ExDateTimeUtils.format(date, "Y/m/d H:i");
	}
	catch(e) {
		if (e instanceof ExDateTimeException) {
			Validator.output(obj, e.message);
		}
		else {
			throw e;
		}
	}
}

//日時： YYYYMMDD hhmiss sss
function setFormatTimestamp(obj) {
//	Validator.init(obj);
	if (obj.value == null || obj.value == "") {
		return;
	}

	try {
		obj.value = Validator.tohalf(obj.value);
		var date = new ExDateTime().parseTimestamp(obj.value, true);
		obj.value = ExDateTimeUtils.format(date, "Y/m/d H:i:s f");
	}
	catch(e) {
		if (e instanceof ExDateTimeException) {
			Validator.output(obj, e.message);
		}
		else {
			throw e;
		}
	}
}

// 時刻： hhmm形式
function setFormatTime(obj) {
//	Validator.init(obj);
	if (obj.value == null || obj.value == "") {
		return;
	}

	try {
		obj.value = Validator.tohalf(obj.value);
		var date = new ExDateTime().parseTime(obj.value);
		obj.value = ExDateTimeUtils.format(date, "H:i");
	}
	catch(e) {
		if (e instanceof ExDateTimeException) {
			Validator.output(obj, e.message);
		}
		else {
			throw e;
		}
	}
}

/*********************************************************************************************************************************************/
/**  数字の補完: Form
/*********************************************************************************************************************************************/
/**
 * 指定された値をフォーマットして指定されたIDのエレメントに設定します。
 * @param id エレメントのID
 * @param value 指定値
 * @param n 小数点以下桁数
 * @param isSeparate カンマ区切りするか
 * @param isOverwrite 上書きするか
 */
function setFormatNumber(id, value, n, isSeparate, isOverwrite) {
	if (value == null || value == undefined || value == "") {
		return;
	}

	var element = document.getElementById(id);
	if (isNullorUndefined(element)) {
		return;
	}
	
	try {	
		
		if (element.value != undefined) {
			if (isOverwrite == false && element.value != "") {
				return;
			}
			var formatNumber = getFormatedNumberString(value, n, isSeparate);	
			element.value = formatNumber;
		} else if (element.innerText != undefined) {
			if (isOverwrite == false && !isNullorUndefinedorEmpty(element.innerText)) {
				return;
			}
			if (isOverwrite == false && !isNullorUndefinedorEmpty(element.textContent)) {
				return;
			}
			var formatNumber = getFormatedNumberString(value, n, isSeparate);	
			element.innerText = formatNumber;
			element.textContent = formatNumber;
		}
	} catch(e) {
		// 何もしない
	}
}

/**
 * 指定された値をフォーマットされた値に変換して返却します。
 * @param value 指定値
 * @param n 小数点以下桁数
 * @param isSeparate カンマ区切りするか
 * @returns フォーマットされた文字列
 */
function getFormatedNumberString(value, n, isSeparate) {
	var fmt;
	if (isSeparate) {
		fmt = "#,##0";
	} else {
		fmt = "0";
	}

	if(n != null && n > 0) {
		fmt += ".";
		for(var i=0 ; i < n; i++) {
			fmt += "0";
		}
	}

	var numValue;
	try {
		var num = new ExNumber(value);
		numValue = num.toFormatString(fmt);
//		if ((n != null && n > 0) || (isSeparate != null && isSeparate == true)) {
			return numValue;
//		}
		return "";
	} catch(e) {
		throw e;
	}
	
}

/*********************************************************************************************************************************************/
/**  Parts: SubWindow関連
/*********************************************************************************************************************************************/
/**
 * モーダルで子画面表示
 *
 * showModalDialogで子画面を開くと子画面内でsubmit不可のためsubWindow.htmlをフレームとして使用する
 * @url : 遷移先URL
 * @titleString : 子画面タイトル
 * @s_width : 子画面幅
 * @s_height : 子画面高さ
 * @return : 子画面からのリターン値
 *
 */
function showSubModalWindow(url, titleString, s_width, s_height) {
	if (url == null || url == '') return '';

	if (s_width == null) s_width = '1024';
	if (s_height == null) s_height = '768';

	var arr = new Array();
	arr[0] = url;
	arr[1] = titleString;
	arr[2] = this;

	var retVal;
	if (window.showModalDialog) {
		retVal = window.showModalDialog('contents/parts/calendar/subWindow.html', arr, 'status:no;scroll:no;help:no;dialogWidth:' + s_width + 'px;dialogHeight:' + s_height + 'px');
	}

	if (typeof retVal == 'undefined') retVal='';
	if (typeof retVal == null) retVal='';

	return retVal;
}

/*
 * カレンダーのモーダルWindowの表示
 *   fromDate : 開始日時項目
 *   toDate   : 終了日時項目（nullを指定した場合はFROMのみの入力）
 *   setType  : 時間入力ありの場合はtrue、無しの場合はfalse
 */
function openCalendar(contextRoot, fromDate, toDate, setType) {
	var url = "";
	if (contextRoot != null && contextRoot != undefined && contextRoot != "") {
		url = contextRoot;
	}

	url += "contents/parts/calendar/calendar.html?";
	url += "start_date=" + fromDate.value;
	url += "&start_date_field=" + fromDate.name;

	var width;
	if (toDate == null) {
		url += "&end_date=";
		url += "&end_date_field=";
		width = 410;
	}
	else {
		url += "&end_date=" + toDate.value;
		url += "&end_date_field=" + toDate.name;
		width = 560;
	}
	if (setType) {
		url += "&time_selector=true";
	}
	else {
		url += "&time_selector=false";
	}
	url += "&seq=" + new Date().getTime();

	var height = '390';


	var arr = new Array();
	arr[0] = url;
	arr[1] = '日付選択';
	arr[2] = this;

	var retVal;
	if (window.showModalDialog) {
		retVal = showModalDialog(url, window, 'status:no;scroll:no;help:no;dialogWidth:' + width + 'px;dialogHeight:' + height + 'px');
	}

	return false;
}

/*********************************************************************************************************************************************/
/**  共通：その他ユーティリティ
/*********************************************************************************************************************************************/
function isArray(obj) {
  return !(
    !obj ||
    (!obj.length || obj.length == 0) ||
    typeof obj != 'object' ||
    !obj.constructor ||
    obj.nodeType ||
    obj.item
 );
}

/**
 * 指定オブジェクトのページ内座標の取得
 * @obj	: 座標を取得するオブジェクト
 */
function getPagePosition(obj) {
	var pos = new Object();
	pos.x = obj.offsetLeft;
	pos.y = obj.offsetTop;

	var parent = obj.parentNode;
	while(parent != null) {
		// TABLE関連のタグのOffset座標をオブジェクトのOffset座標に加算する
		if (parent.tagName == "TABLE" || parent.tagName == "TD" || parent.tagName == "TH") {
			pos.x += parent.offsetLeft;
			pos.y += parent.offsetTop;
		}
		parent = parent.parentNode;
	}
	return pos;
}

function isEmpty(str) {
	if (str == null || trim(str) == "" || isUndefined(str)) return true;
	return false;
}

function createElement(tag, id, name) {
	var str;
	var element = null;
	if (isIE()) {
		str = "<" + tag;
		if (!isEmpty(id)) {
			str += " id='" + id + "'";
		}
		if (!isEmpty(name)) {
			str += " name='" + name + "'";
		}
		str += ">";
		element = document.createElement(str);
	}
	else {
		element = document.createElement(tag);
		if (!isEmpty(id)) {
			element.setAttribute("id", id);
		}
		if (!isEmpty(name)) {
			element.setAttribute("mame", name);
		}
	}
	return element;
}

function isUndefined(obj) {
	if (obj == null) return true;
	return typeof obj == "undefined";
}

function isDefined(obj) {
	return !isUndefined(obj);
}

/******************************************************************************************************************************/
/**  Keybord Eventクラス
/******************************************************************************************************************************/
var Keybord = {
	initialize: function() {
		setEventFunction(document, "onkeydown", this.onkeydown);
		setEventFunction(document, "onkeyup", this.onkeyup);
	},

	onkeydown: function(e) {
	    // Mozilla(Firefox, NN) and Opera
	    if (e != null) {
	    	Keybord.keycode = e.which;
			Keybord.ctrl = isUndefined(e.modifiers) ? e.ctrlKey : e.modifiers & Event.CONTROL_MASK;
			Keybord.shift = isUndefined(e.modifiers) ? e.shiftKey : e.modifiers & Event.SHIFT_MASK;
	    } else {
    	    Keybord.keycode = event.keyCode;
        	Keybord.ctrl = event.ctrlKey;
        	Keybord.shift = event.shiftKey;
    	}
	},

	onkeyup: function(e) {
		Keybord.ctrl = false;
		Keybord.shift = false;
	}
}

/******************************************************************************************************************************/
/**  クリックイベント発生関数
/******************************************************************************************************************************/
function fireClick(id) {
	var obj = document.getElementById(id);
	if (obj.fireEvent) {
		obj.fireEvent("onclick");
	}
	else if (obj.dispatchEvent) {
		var evt = document.createEvent("MouseEvents");	// マウスイベントを作成
		evt.initMouseEvent("click", true, true, window, 1, 1, 1, 1, 1, false, false, false, false, 0, obj);				// イベントの詳細を設定
		obj.dispatchEvent(evt);
	}
}

///**
// * 入力チェック
// */
//function formCheckAll(form) {
//	if (form == null || form == undefined) {
//		return false;
//	}
//
//	var isError = false;
//	for (var i = 0 ;i < form.elements.length; i++) {
//		obj = form.elements[i];
//		var type = obj.type;
//		var className = obj.className;
//		var value = obj.value;
//
//		if (type == 'text' || type == 'password') {
//			if (className.indexOf("REQUIRED_INPUT") >= 0) {
//				Validator.init(obj);
//				var checked = Validator.check(obj, "input");
//				if (!checked) {
//					isError = true;
//					continue;
//				}
//			}
//
//		} else if (type == 'textarea') {
//			if (className.indexOf("REQUIRED_TEXT_AREA") >= 0) {
//				Validator.init(obj);
//				var checked = Validator.check(obj, "input");
//				if (!checked) {
//					isError = true;
//					continue;
//				}
//			}
//		} else if (type == 'select-one') {
//			if (className.indexOf("REQUIRED_COMBO") >= 0) {
//				Validator.init(obj);
//				var checked = Validator.check(obj, "input");
//				if (!checked) {
//					isError = true;
//					continue;
//				}
//			}
//		} else if (type == 'select-multiple') {
//			if (className.indexOf("REQUIRED_SELECTLIST") >= 0) {
//				Validator.init(obj);
//				var checked = Validator.check(obj, "input");
//				if (!checked) {
//					isError = true;
//					continue;
//				}
//			}
//		}
//		if (value != null && value != undefined && value != "") {
//			if (type == 'text' || type == 'password') {
//			    if (className.indexOf("MAIL_ADDRESS") >= 0) {
//					Validator.init(obj);
//					var checked = Validator.check(obj, "mail");
//					if (!checked) {
//						isError = true;
//					}
//			    } else if (className.indexOf("HALF_ENGLISH_NUMBER") >= 0) {
//					Validator.init(obj);
//					var checked = Validator.check(obj, "alphabetNumber");
//					if (!checked) {
//						isError = true;
//					}
//			    } else if (className.indexOf("HALF_ENGLISH") >= 0) {
//					Validator.init(obj);
//					var checked = Validator.check(obj, "alphabet");
//					if (!checked) {
//						isError = true;
//					}
//			    } else if (className.indexOf("FULL_PITCH_CHAR_KANA") >= 0) {
//					Validator.init(obj);
//					var checked = Validator.check(obj, "kana");
//					if (!checked) {
//						isError = true;
//					}
//			    } else if (className.indexOf("FULL_PITCH_CHAR") >= 0) {
//					Validator.init(obj);
//					var checked = Validator.check(obj, "zenkaku");
//					if (!checked) {
//						isError = true;
//					}
//				} else if (className.indexOf("NUMBER") >= 0) {
//					Validator.init(obj);
//					var checked = Validator.check(obj, "num");
//					if (!checked) {
//						isError = true;
//					}
//				} else if (className.indexOf("DATE_YYMM") >= 0) {
//					// YYYY/MM
//					Validator.init(obj);
//					try {
//						var date = new ExDateTime().parseYM(value);
//					} catch (e) {
//						isError = true;
//						Validator.output(obj, e.message);
//					}
//				} else if (className.indexOf("DATE_TIME") >= 0 || obj.className.indexOf("TIMESTAMP") >= 0) {
//					// YYYY/MM/DD hh:mi:ss
//					Validator.init(obj);
//					try {
//						var date = new ExDateTime().parseDate(value, true);
//					} catch (e) {
//						isError = true;
//						Validator.output(obj, e.message);
//					}
//				} else if (className.indexOf("DATE") >= 0) {
//					// YYYY/MM/DD
//					Validator.init(obj);
//					try {
//						var date = new ExDateTime().parseDate(value, false);
//					} catch (e) {
//						isError = true;
//						Validator.output(obj, e.message);
//					}
//				} else if (className.indexOf("TIME") >= 0) {
//					Validator.init(obj);
//					// hh:mi
//					try {
//						var date = new ExDateTime().parseTime(value);
//					} catch (e) {
//						isError = true;
//						Validator.output(obj, e.message);
//					}
//				}
//			}
//		}
//	}
//	return isError;
//}

/**
 * 標準リストを制御するクラス
 */
var StandardList = function() {
	this.initialize.apply(this, arguments);
}

StandardList.prototype = {
	/**
	 * テーブルID
	 */
	tableId: null,

	/**
	 * 対象テーブル
	 */
	tableElement: null,

	/**
	 * 選択行オブジェクト
	 */
	selectedRow: null,

	/**
	 * 移動時の行のスタイル
	 */
	moveRowStyle: "MOUSE_OVER_DETAILS_ROW",

	/**
	 * 選択された行のスタイル
	 */
	selectRowStyle: "SELECT_DETAILS_ROW",

	/**
	 * 初期化
	 * @param tableId テーブルのID
	 * @param table 対象テーブル
	 * @param selectedRowId 初期選択行
	 */
    initialize: function(tableId, table, selectedRowId) {
		this.tableId = tableId;
		this.tableElement = table;
		if (selectedRowId != null && selectedRowId != undefined) {
			this.selectRowById(selectedRowId);
		}
	},

	/**
	 * 行をRowIDから指定して選択します。
	 * @param id RowID
	 */
	selectRowById: function(id) {
		if (this.tableElement != null && this.tableElement != undefined && id != null && id != undefined) {
			var indexList = id.replace(this.tableId, "").split("_");
			var rows;
			if (indexList.length > 2) {
				rows = this.tableElement[indexList[1]].rows;
			} else {
				rows = this.tableElement[0].rows;
			}
			var row;
			if (!isNullorUndefined(rows)) {
				row = rows.namedItem(id);
			} else {
				row = document.getElementById(id);
			}
	    	if (!isNullorUndefined(row)) {
	    		this.selectRow(row);
	    		return true;
	    	}
		}
		return false;
	},
	
	/**
	 * 行の選択
	 * @param object 選択された行オブジェクト
	 */
	selectRow: function(object) {
		var selectedRow = this.getSelectedRowInSameDetails(object);
		if (!isNullorUndefined(selectedRow)) {
			this.changeRowStyle(selectedRow, "");
		}
		this.selectedRow = this.setSelectedRowInSameDetails(object);
		this.changeRowSelectColor(object);
	},
	
	/**
	 * 同明細の選択行の取得
	 * @param object 選択された行オブジェクト
	 */
	getSelectedRowInSameDetails: function(object) {
		var indexList = object.id.replace(this.tableId, "").split("_");
		var selectedRow;
		if (indexList.length > 2) {
			selectedRow = this.getSelectedRow(this.selectedRow, indexList, 1);
		} else {
			selectedRow = this.selectedRow;
		}
		return selectedRow;
	},
	
	/**
	 * 同明細の選択行の取得(再帰処理部)
	 * @param object 選択された行オブジェクト
	 * @param indexList インデックスリスト
	 */
	getSelectedRow: function(elements, indexList, index) {
		if (index == indexList.length - 1) {
			return elements;
		} else {
			if (!isNullorUndefined(elements)) {
				return this.getSelectedRow(elements[indexList[index]], indexList, index + 1);
			}
		}
	},

	/**
	 * 同明細の選択行の取得
	 * @param object 選択された行オブジェクト
	 */
	setSelectedRowInSameDetails: function(object) {
		var indexList = object.id.replace(this.tableId, "").split("_");
		if (indexList.length > 2) {
			return this.setSelectedRow(this.selectedRow, indexList, 1, object, "");
		} else {
			return object;
		}
	},
	
	/**
	 * 同明細の選択行の取得(再帰処理部)
	 * @param elements 現在選択行オブジェクト配列
	 * @param indexList インデックスリスト
	 * @param index インデックスリストのインデックス
	 * @param object 選択された行オブジェクト
	 * @param indexSymbol インデックスシンボル
	 */
	setSelectedRow: function(elements, indexList, index, object, indexSymbol) {
		if (index == indexList.length - 1) {
			var indexElement = document.getElementById(this.tableId + "SelectedIndex" + indexSymbol);
			indexElement.value = indexList[index];
			return object;
		} else {
			if (isNullorUndefined(elements)) {
				elements = new Array();
			}
			indexSymbol = indexSymbol + "_" + indexList[index];
			elements[indexList[index]] = this.setSelectedRow(elements[indexList[index]], indexList, index + 1, object, indexSymbol);
			return elements;
		}
	},

	/**
	 * グリッドの行を移動する際に色を設定する
	 * @param object 行オブジェクト
	 */
	setRowMoveColor: function(object) {
		if (!this.checkSelectedRow(object)) {
			this.changeRowStyle(object, this.moveRowStyle);
		}
	},

	/**
	 * グリッドの行を離れる際、行の色をもとに戻す
	 * @param object 行オブジェクト
	 */
	resetRowMoveColor: function(object) {
		if (!this.checkSelectedRow(object)) {
			this.changeRowStyle(object, "");
		}
	},
	
	/**
	 * 対象行が選択中かをチェックする
	 * @param object 行オブジェクト
	 */
	checkSelectedRow: function(object) {
		var selectedRow = this.getSelectedRowInSameDetails(object);
		if (object == selectedRow) {
			return true;
		} else {
			return false;
		}
	},

	/**
	 * 選択されたグリッドの行の色を変更する
	 * @param object 行オブジェクト
	 */
	changeRowSelectColor: function(object) {
		this.changeRowStyle(object, this.selectRowStyle);
	},

	/**
	 * 行の色を変更する
	 * @param object 行オブジェクト
	 * @param className 色(空白の時は元の色(CSSなどで定義されている色)に戻す)
	 */
	changeRowStyle: function(object, changeStyle) {
		if (object == undefined) {
			return;
		}

		if (object.cells == undefined) {
			return;
		}

		for (var i = 0; i < object.cells.length; i++) {
			var cell = object.cells[i];
			
			this.changeCellStyle(cell, changeStyle);						
			if (cell.hasChildNodes()) {
				for (var j = 0; j < cell.childNodes.length; j++) {
					var childNode = cell.childNodes[j];
					if (childNode.nodeType == 1) {
						if (childNode.type == "text" || childNode.type == "password" || childNode.type == "select-one" || childNode.type == "select-multiple") {
							this.changeCellStyle(childNode, changeStyle);
						}
					}
				}
			}
		}
	},
	
	changeCellStyle: function(cell, changeStyle) {
		if (cell == null || cell == undefined) {
			return;
		}
		
		var className = cell.className;
		if (changeStyle != "") {
			if (className.indexOf(this.moveRowStyle) >= 0 && changeStyle == this.selectRowStyle) {
				// 選択時は、移動スタイルをクリア
				className = className.replace(this.moveRowStyle, "");
			} 
			
			if (className.indexOf(this.selectRowStyle) >= 0 && changeStyle == this.moveRowStyle) {
				// 選択スタイルは移動スタイルで上書きしない
			} else {
				if (className.indexOf(changeStyle) < 0) {
					className = className + " " + changeStyle;
				}
			}
		} else {
			// クリア
			if (className != null && className != undefined && className != "") {
				className = className.replace(this.selectRowStyle, "");
				className = className.replace(this.moveRowStyle, "");
			}
		}

		cell.className = className;
	},

	/**
	 * セルの色を親ノードと同一の色にします。
	 */
	cellColorToParentNodeColor: function(cell) {
		if (cell != null && cell != undefined && cell.parentNode) {
			cell.style.background = cell.parentNode.style.background;
		}
	}
}

/**
 * 指定されたURLのサブウィンドウダイアログを表示します。
 * @param url URL(encodeされます)
 * @param width ダイアログ幅(px)
 * @param height ダイアログ高さ(px)
 * @param target Window Target名
 */
function openWindow(url, width, height, target) {
	return openModalWindow(url, width, height, target);
}

/**
 * ポップアップダイアログを閉じます。
 */
function windowClose() {
	if (isIE()) {
        window.opener = "myself";
    }
    window.close();
}

/**
 * 指定されたURLのサブウィンドウダイアログをモードレスで表示します。
 * @param url URL(encodeされます)
 * @param width ダイアログ幅(px)
 * @param height ダイアログ高さ(px)
 * @param target Window Target名
 */
function openModelessWindow(url, width, height, target) {
	if (url == null || url == undefined) {
		return;
	}
	
	width = width || 1024;
	height = height || 768;
	if (isNullorUndefined(target)) {
		target = "_blank";
	}

	var win = window.open(encodeURI(url), target, "width=" + width + ", height=" + height + ",scrollbars=yes,resizable=yes,status=no");
	if (isIE()) {
		// IEの場合、Windowが後ろに回りこんでしまうことがあるので、100msec後にWindowをActiveにする
	    setTimeout(function() {
			win.focus();
	    }, 100);
	} else {
		win.focus();
	}
	
	return win;
}

/**
 * 指定されたURLのサブウィンドウダイアログをモーダルで表示します。
 * @param url URL(encodeされます)
 * @param width ダイアログ幅(px)
 * @param height ダイアログ高さ(px)
 * @param target Window Target名
 */
function openModalWindow(url, width, height, target) {
	if (url == null || url == undefined) {
		return;
	}
	width = width || 1024;
	height = height || 768;
	
	var option = "status:no;resizable:yes;scroll:yes;help:no;dialogWidth:" + width + "px;dialogHeight:" + height + "px";
	return showModalDialog(encodeURI(url), window, option);
}

/**
 * GETパラメータを取得します。
 * @returns GETパラメータの連想配列
 */
function getGetParameter(){
	if(location.search.length > 1) {
		var get = new Object();
	    var ret = location.search.substr(1).split("&");
	    for(var i = 0; i < ret.length; i++) {
	    	var r = ret[i].split("=");
	    	get[r[0]] = r[1];
	    }
	    return get;
	} else {
	    return new Object();
    }
}

/**
 * 親のフィールドの指定したフィールドに指定した値を設定します。<br>
 * その際、指定があれば、親のフィールドにフォーカスを移動します。
 * @param parentId 親のフィールドID
 * @param value 設定する値
 * @param overwrite 上書きするか
 * @return 設定がされた場合true
 */
function setValueToParentWindowItem(parentId, value, overwrite) {
	var openerDocument = getOpenerDocument();
	if (openerDocument == null || openerDocument == undefined) {
		return false;
	}
	
	var parentItem = openerDocument.getElementById(parentId);
	if (parentItem != null && parentItem != undefined) {
		if (parentItem.value != undefined) {
			if (overwrite || (!overwrite && parentItem.value == "")) {
				parentItem.value = value;
			}
		} else if (parentItem.innerText != undefined) {
			if (overwrite || (!overwrite && parentItem.innerText == "")) {
				parentItem.innerText = value;
			}
		} else if (parentItem.textContent != undefined) {
			if (overwrite || (!overwrite && parentItem.textContent == "")) {
				parentItem.textContent = value;
			}
		}
		return true;
	} else {
		// radioの可能性
		var elements = openerDocument.getElementsByName(parentId);
		if (!isNullorUndefined(elements) && elements.length > 0) {
			if (elements[0].type == "radio") {
				checkRadioElement(elements, value);
				return true;
			}
		}	
		return false;
	}
}

/**
 * 親のフィールドの指定したフィールドにフォーカスを移動します。
 * @param parentId 親のフィールドID
 */
function setFocusToParentWindowItem(parentId) {
	var openerDocument = getOpenerDocument();
	if (openerDocument == null || openerDocument == undefined) {
		return;
	}
	var parentItem = openerDocument.getElementById(parentId);
	if (parentItem != null && parentItem != undefined) {
		parentItem.focus();
	}
}

/**
 * 親WindowのDocumentオブジェクトを取得します。
 * @returns 親WindowのDocumentオブジェクト
 */
function getOpenerDocument() {
	if(window.dialogArguments) {
		opener = window.dialogArguments;
	} else if(window.opener){
		// FireFox対応
		opener = window.opener;
	} else if(window.parent.opener){
		// FireFox対応
		opener = window.parent.opener;
	}
	
	if (opener == null) {
		return null;
	}
	var openerDocument = opener.document;
	return openerDocument;
}

/**
 * 指定されたフォームアイテムに対し、クラス名の設定を行います。
 * @param id フォームID
 * @param className 指定クラス名
 */
function changeClassName(id, className) {
	var element = document.getElementById(id);
	if (element != null && element != undefined && element.className != undefined) {
		element.className = className;
	}
}

/**
 * 指定されたフォームアイテムのクラス名をクリアします。
 * @param id フォームID
 */
function clearClassName(id) {
	changeClassName(id, "");
}

/**
 * エレメントの値のコピーを行います。
 * @param from コピー元
 * @param to コピー先
 * @param isOverwrite 上書きするか
 */
function copyValueElements(from, to, isOverwrite) {
	var fromElement = document.getElementById(from);
	var toElement = document.getElementById(to);
	if (fromElement != null && fromElement != undefined && toElement != null && toElement != undefined) {
		if (isOverwrite) {
			toElement.value = fromElement.value;
		} else {
			if (toElement.value == null || toElement.value == "") {
				toElement.value = fromElement.value;
			}
		}
	}
}

/**
 * 指定されたIDのコンボボックスを指定された値の選択値に切り替えます。
 * @param id コンボボックスID
 * @param value 指定値
 */
function selectCombobox(id, value) {
	var element = document.getElementById(id);
	var isSelect = false;
	if (element != null && element != undefined && element.options != undefined) {
		for (var i = 0; i < element.options.length; i++) {
			if (value == element.options[i].value) {
				element.selectedIndex = i;
				isSelect = true;
			}
		}
		if (!isSelect) {
			element.selectedIndex = -1;
		}
	}
}

/**
 * 対象エレメントの値が空でないことを確認します。
 * @param element エレメント
 * @returns 空の場合true
 */
function isElementValueEmpty(element) {
	if (element != null && element != undefined && element.value != null && 
			element.value != undefined && element.value != "") {
		return false;
	} else {
		return true;
	}
}

/**
 * 対象オブジェクトに紐づく子ノードを全て削除します。
 * コンボボックスのオプションをクリアする場合などに利用します。
 * @param x 対象オブジェクト
 */
function removeChildren(x) {
	if (isNullorUndefined(x)) {
		return;
	}
	if (x.hasChildNodes()) {
		while (x.childNodes.length > 0) {
			x.removeChild(x.firstChild)
		}
	}
}

/**
 * 対象オブジェクトがNullか undefined か判定し、Nullか undefinedである場合trueを返却します。
 * @param obj オブジェクト
 * @returns 対象オブジェクトがNullか undefinedである場合true
 */
function isNullorUndefined(obj) {
	if (obj == null || obj == undefined) {
		return true;
	} else {
		return false;
	}
}

/**
 * 対象オブジェクトがNullか undefinedか 空文字か判定し、Nullか undefinedか 空文字である場合trueを返却します。
 * @param obj オブジェクト
 * @returns 対象オブジェクトがNullか undefinedか 空文字である場合true
 */
function isNullorUndefinedorEmpty(obj) {
	if (obj == null || obj == undefined || obj === "") {
		return true;
	} else {
		return false;
	}
}


/**
 * エレメントのIDを指定し、指定されたIDのエレメント数値を加算して返却します。
 * @param 加算されるエレメントのID(複数指定可能)
 * @returns 加算された結果 
 */
function plusForElements() {
	var total = 0;
	var isAllNaN = true;
	for (var i = 0; i < arguments.length; i++) {
		var id = arguments[i];
		var number = getElementValueNumber(id);
		if (!isNaN(number)) {
			isAllNaN = false;
			total += number;
		}
	}
	
	if (isAllNaN) {
		return Number.NaN;
	} else {
		return total;
	}
}

/**
 * エレメントのIDを指定し、指定されたIDのエレメント数値を減算して返却します。
 * 第一引数から、第二引数以降の値を減算します。
 * @param 減算されるエレメントのID(複数指定可能)
 * @returns 減算された結果 
 */
function subtractForElements() {
	var total = 0;
	var isAllNaN = true;
	for (var i = 0; i < arguments.length; i++) {
		var id = arguments[i];
		var number = getElementValueNumber(id);
		if (!isNaN(number)) {
			if (i == 0) {
				total = number;
			} else {
				total -= number;
			}
			isAllNaN = false;
		}
	}
	
	if (isAllNaN) {
		return Number.NaN;
	} else {
		return total;
	}
}

/**
 * エレメントのIDを指定し、指定されたIDのエレメント数値を乗算して返却します。
 * @param 乗算されるエレメントのID(複数指定可能)
 * @returns 乗算された結果 
 */
function multiplyForElements() {
	var total = 0;
	for (var i = 0; i < arguments.length; i++) {
		var id = arguments[i];
		var number = getElementValueNumber(id);
		if (isNaN(number)) {
			return Number.NaN;
		}
		
		if (number == 0) {
			// 0は何回かけても0
			return 0;
		}
		
		if (total == 0) {
			total = number;
		} else {
			total *= number;
		}
	}
	return total;
}

/**
 * エレメントのIDを指定し、指定されたIDのエレメント数値を除算して返却します。
 * 第一引数から、第二引数以降の値を除算します。
 * @param 除算されるエレメントのID(複数指定可能)
 * @returns 除算された結果 
 */
function divideForElements() {
	var total = 0;
	for (var i = 0; i < arguments.length; i++) {
		var id = arguments[i];
		var number = getElementValueNumber(id);
		if (isNaN(number)) {
			return Number.NaN;
		}

		if (i == 0) {
			total = number;
			if (number == 0) {
				// 0は何回割っても0
				return 0;
			}
		} else {
			if (number == 0) {
				// 0割り不可
				return Number.NaN;
			}
			total /= number;
		}
	}	
	return total;
}


/**
 * エレメントのIDを指定し、数値を取得します。
 * エレメントの値が数値でない場合、Number.NaNを返却します。
 * @param id エレメントのID
 * @returns エレメントの数値(or Number.NaN)
 */
function getElementValueNumber(id) {
	var idObjStr = getElementValue(id);
	if (idObjStr != "") {
		var idNumber = new Number(removeCanma(idObjStr));
		if(!isNaN(idNumber)) {
			return idNumber;
		}
	}
	return Number.NaN;
}

/**
 * エレメントのIDを指定し、値を文字列で取得します。
 * @param id エレメントのID
 * @returns エレメントの文字列
 */
function getElementValue(id) {
	var obj = document.getElementById(id);
	return getValueFromElement(obj);	
}

/**
 * エレメントから値を文字列で取得します。
 * @param element エレメント
 * @returns エレメントの文字列
 */
function getValueFromElement(element) {
	if (isNullorUndefined(element)) {
		return "";
	}
	
	if (!isNullorUndefined(element.value)) {
		return element.value;
	} else if (!isNullorUndefined(element.innerText)) {
		return element.innerText;
	} else if (!isNullorUndefined(element.textContent)) {
		return element.textContent;
	} else {
		return "";
	}
}

/**
 * 指定したエレメントのvalueを書き換えます。(text/checkbox/radio/hidden etc.)
 * @param id エレメントのID
 * @param value 設定する値
 * @param isOverwrite 上書きするか
 */
function setValue(id, value, isOverwrite) {
	var element = document.getElementById(id);
	
	if (!isNullorUndefined(element)) {
		if (isNullorUndefined(value)) {
			value = "";
		}
	
		if (isOverwrite) {
			element.value = value;
		} else {
			if (element.value == null || element.value == "") {
				element.value = value;
			}
		}
	} else {
		// radioの可能性
		var elements = document.getElementsByName(id);
		if (!isNullorUndefined(elements) && elements.length > 0) {
			if (elements[0].type == "radio") {
				checkRadioElement(elements, value);
			}
		}	
	}
}

/**
 * valueと同一のエレメントにチェックをつけます。
 * @param elements radioのエレメント配列
 * @param value チェックする値
 */
function checkRadioElement(elements, value) {
	if (isNullorUndefined(elements)) {
		return;
	}

	for (var i = 0; i < elements.length; i++) {
		var radioElement = elements[i];
		if (radioElement.value == value) {
			radioElement.checked = true;
			break;
		}
	}
}

/**
 * 指定したエレメントのinnerTextを書き換えます。(label/textarea/pre/span/div etc.)
 * @param id エレメントのID
 * @param value 設定する値
 * @param isOverwrite 上書きするか
 */
function setInnerText(id, value, isOverwrite) {
	var element = document.getElementById(id);
	if (element != null && element != undefined) {
		if (isNullorUndefined(value)) {
			value = "";
		}
		if (isOverwrite) {
			element.innerText = value;
			element.textContent = value;
		} else {
			if (isNullorUndefined(element.innerText)) {
				element.innerText = value;
			} else if (isNullorUndefined(element.textContent)) {
				// FireFox
				element.textContent = value;
			}
		}
	}
}

/**
 * 指定したエレメントのsrcを書き換えます。(img)
 * @param id エレメントのID
 * @param value 設定する値
 * @param isOverwrite 上書きするか
 */
function setSrc(id, value, isOverwrite) {
	var element = document.getElementById(id);
	if (element != null && element != undefined) {
		if (isNullorUndefined(value)) {
			value = "";
		}
		if(isOverwrite) {
			element.src = value;
		} else {
			if(element.src == null || element.src == "") {
				element.src = value;
			}
		}
	}
}

/**
 * 指定したエレメントのvalueをコピーします。(label/textarea/pre/span/div etc.)
 * @param from コピー元エレメントのID
 * @param to コピー先エレメントのID
 * @param isOverwrite 上書きするか
 */
function copyInnerText(from, to, isOverwrite) {
	var fromElement = document.getElementById(from);
	var toElement = document.getElementById(to);
	
	if (!isNullorUndefined(fromElement) && !isNullorUndefined(toElement)) {
		if(isOverwrite) {
			if (isNullorUndefined(fromElement.innerText)) {
				toElement.innerText = fromElement.innerText;
			} else if (isNullorUndefined(fromElement.textContent)) {
				// FireFox
				toElement.textContent = fromElement.textContent;
			}
		} else {
			if(toElement.innerText == null || toElement.innerText == "") {
				if (isNullorUndefined(fromElement.innerText)) {
					toElement.innerText = fromElement.innerText;
				} else if (isNullorUndefined(fromElement.textContent)) {
					// FireFox
					toElement.textContent = fromElement.textContent;
				}
			}
		}
	}
}

/**
 * 指定したエレメントのsrcをコピーします。(img)
 * @param from コピー元エレメントのID
 * @param to コピー先エレメントのID
 * @param isOverwrite 上書きするか
 */
function copySrc(from, to, isOverwrite) {
	var fromElement = document.getElementById(from);
	var toElement = document.getElementById(to);
	
	if (fromElement != null && fromElement != undefined && toElement != null && toElement != undefined) {
		if(isOverwrite) {
			toElement.src = fromElement.src;
		} else {
			if(toElement.src == null || toElement.src == "") {
				toElement.src = fromElement.src;
			}
		}
	}
}

/**
 * GETパラメータを取得します。
 * @returns GETパラメータの連想配列
 */
function getRequestParameter() {
	if(location.search.length > 1) {
		var get = new Object();
	    var ret = location.search.substr(1).split("&");
	    for(var i = 0; i < ret.length; i++) {
	    	var r = ret[i].split("=");
	    	get[r[0]] = r[1];
	    }
	    return get;
	} else {
    return;
    }
}

/**
 * 名前を指定した要素の先頭を取得
 * @param name 要素名
 * @return 取得したelement
 */
function getFirstElement(name){
    var elements = document.getElementsByName(name);
    if (isNullorUndefined(elements) || elements.length == 0){
        return undefined;
    }
    return elements[0];
}

/**
 * Debug用テキスト表示
 */
function writeDebugText(message) {
	var debugArea = document.createElement("div");
	var message = document.createTextNode(ExDateTimeUtils.format(new Date(), "Y/m/d H:i:s : ") + message);
	debugArea.appendChild(message);
	document.body.appendChild(debugArea);
}

/**
 * 指定したIDのint値を取得します。
 * @param id ID
 * @returns int値
 */
function getIntValue(id) {
    var element = document.getElementById(id);
    if (element != null && element != undefined) {
        var intValue = parseInt(element.value);
        if (intValue != NaN) {
        	return intValue;
        } else {
        	return 0;
        }
    } else {
    	return 0;
    }
}

/**
 * 先頭を大文字に変換して返却します。
 * @param str 対象文字列
 * @returns 対象文字列の先頭を大文字にした文字列
 */
function capitalize(str) {
	if (isNullorUndefinedorEmpty(str)) {
		return "";
	}
	
	return str.charAt(0).toUpperCase() + str.slice(1);
}

/**
 * 有効な生年月日かどうか判定します。
 * @param targetDateText 判定対象日付
 * @param isIncludeToday 有効日付に今日を含めるか
 * @returns 有効ならtrue 無効ならfalse 
 */
function isAvailableBirthDay(targetDateText, isIncludeToday) {
	var compareResult = compareDateWithToday(targetDateText);
	if (isIncludeToday) {
		if (compareResult == 0 || compareResult == 1) {
			return true;
		} else {
			return false;
		}
	} else {
		if (compareResult == 1) {
			return true;
		} else {
			return false;
		}
	}
}

/**
 * 本日日付と比較して、比較対象日時の方が未来日の時は-1 等しい時は0  過去日の時は 1を返却します。
 * @param targetDateText 比較対象日時
 */
function compareDateWithToday(targetDateText) {
	var date = new Date(targetDateText);
	var now = new Date();
	var today = new Date(now.getFullYear(), now.getMonth(), now.getDate());
	return compareDate(today, date);
}

/**
 * 2つの日付を比較して日付1の方が未来日の時は1 等しい時は0  過去日の時は -1を返却します。
 * @param targetDate1 日付1
 * @param targetDate2 日付2
 * @returns 比較結果
 */
function compareDate(targetDate1, targetDate2) {
	if (targetDate1 > targetDate2) {
		return 1;
	} else if ((targetDate1 - targetDate2) == 0){
		return 0;
	} else {
		return -1;
	}
}

/**
 * 複数選択用のリストが少なくとも一つ選択されているか判定します。
 * @param id 項目ID
 * @param indexSymbol 項目のインデックスシンボル
 * @returns 複数選択用のリストが少なくとも一つ選択されている場合true
 */
function isMultipleSelect(id, indexSymbol) {
	var targetId = id;
	if (!isNullorUndefinedorEmpty(indexSymbol)) {
		targetId = id + indexSymbol;
	}
    var element = document.getElementById(targetId);
    if (isNullorUndefined(element)) {
    	return;
    }
	if (isNullorUndefined(element.multiple) || !element.multiple) {
		return;
	}
    var multipleSelects = new Array();
    for (var i = 0; i < element.options.length; i++) {
    	if (element.options[i].selected) {
    		return true;
    	}
    }
    return false;
}

/**
 * 複数選択用のリストの選択を全て解除します。
 * @param id 項目ID
 * @param indexSymbol 項目のインデックスシンボル
 */
function releaseMultipleSelect(id, indexSymbol) {
	var targetId = id;
	if (!isNullorUndefinedorEmpty(indexSymbol)) {
		targetId = id + indexSymbol;
	}

    var element = document.getElementById(targetId);
    if (isNullorUndefined(element)) {
    	return;
    }
	if (isNullorUndefined(element.multiple) || !element.multiple) {
		return;
	}
    var multipleSelects = new Array();
    for (var i = 0; i < element.options.length; i++) {
    	element.options[i].selected = false;
    }
}

/**
 * 複数選択用のリストで選択されている値をリストで取得します。
 * @param id 項目ID
 * @param indexSymbol 項目のインデックスシンボル
 * @returns 選択されている値のリスト
 */
function getMultiSelectValues(id, indexSymbol) {
	var targetId = id;
	if (!isNullorUndefinedorEmpty(indexSymbol)) {
		targetId = id + indexSymbol;
	}
	
    var element = document.getElementById(targetId);
    if (isNullorUndefined(element)) {
    	return;
    }
	if (isNullorUndefined(element.multiple) || !element.multiple) {
		return;
	}
	
	var values = new Array();
    for (var i = 0; i < element.options.length; i++) {
    	var option = element.options[i];
    	if (option.selected) {
    		values.push(option.value);
    	}
    }
    return values;
}
