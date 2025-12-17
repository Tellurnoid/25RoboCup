var Validator = {
	errorId: '__error_',
	existsErrorId: 'isExistsError',
	errorGroupClassName: 'alertMsg alertMsg-error',
	check : function(field, reg, extra) {
		var response;
		var rule = this.rule;
		rule.field = field;
		rule.value = field.value;
		rule.extra = extra;

		if (reg == "input") {
			response = rule.input();
		} else if (reg && !response && rule.value != '') {
			reg = reg.replace(/^!/, '');

			var mode = reg.split(/\s+/);
			for ( var i = 0, m; m = mode[i]; i++) {
				m = m.replace(/([\d?\.\-:]+)?$/, '');
				response = rule[m](RegExp.$1);
				if (response) {
					break;
				}
			}
		}

		if (response) {
			this.output(field, response);
			return false;
		} else {
			return true;
		}
	},

	checkTel : function(telId, telNo) {
		var response;
		var rule = this.rule;
		rule.value = telNo;

		response = rule.tel();
		if (response) {
			this.outputbyId(telId, response);
			return false;
		} else {
			return true;
		}
	},
	
	checkFax : function(faxId, faxNo) {
		var response;
		var rule = this.rule;
		rule.value = faxNo;

		response = rule.fax();
		if (response) {
			this.outputbyId(faxId, response);
			return false;
		} else {
			return true;
		}
	},

	checkZip : function(zipId, zipNo) {
		var response;
		var rule = this.rule;
		rule.value = zipNo;

		response = rule.zip();
		if (response) {
			this.outputbyId(zipId, response);
			return false;
		} else {
			return true;
		}
	},
	
	checkKana : function(kanaId, text, item) {
		var response;
		var rule = this.rule;
		rule.value = text;

		response = rule.kana(item);
		if (response) {
			this.outputbyId(kanaId, response);
			return false;
		} else {
			return true;
		}
	},

	allclose : function(form) {
		for (var i = 0, f; f = form[i]; i++) {
			this.init(f);
		}
	},

	init : function(field) {
		if (field == null || field == undefined) {
			return;
		}

		var id = field.id;
		if (field.type == 'radio' || field.type == 'checkbox') {
			id = id.replace(/_0$/, '');
		}
		
		var className = field.className;
		if (!isNullorUndefined(className)) {
			if (className.indexOf("TEL_1") >= 0 || className.indexOf("ZIP_1") >= 0) {
				id = id.replace('Pre', '');
			} else if (className.indexOf("TEL_2") >= 0) {
				id = id.replace('Middle', '');
			} else if (className.indexOf("TEL_3") >= 0 || className.indexOf("ZIP_2") >= 0) {
				id = id.replace('Post', '');
			}
		}	
		
		this.initbyId(id);
	},

	initbyId : function(id, msg) {
		var errorId = this.errorId + id;
		var errorElement = document.getElementById(errorId);
		if (errorElement != null && errorElement != undefined) {
			if (errorElement.innerHTML != undefined) {
				errorElement.innerHTML = "";

				if(!this.isExistErrorMessageWithScreenError()){
					var existsErrorElement = document.getElementById(this.existsErrorId);
					if (existsErrorElement != null && existsErrorElement != undefined) {
						existsErrorElement.innerHTML = "";
					}
					
					var errorGroup = document.getElementById("errorGroupSurround");
					if (errorGroup != null && errorGroup != undefined) {
						errorGroup.className = "";
					}
				}
			}
		}
	},

	output : function(field, msg) {
		if (field == null || field == undefined || msg == null || msg == undefined || msg == "") {
			return;
		}

		var id = field.id;
		if (field.type == 'radio' || field.type == 'checkbox') {
			id = id.replace(/_0$/, '');
		}
		this.outputbyId(id, msg);
	},

	outputbyId : function(id, msg) {
		var errorId = this.errorId + id;
		var errorElement = document.getElementById(errorId);
		if (errorElement != null && errorElement != undefined) {
			if (this.isExistSameErrorThisChildElements(errorElement, msg)) {
				return;
			}
		
			var outputError = document.createElement('span');
			outputError.className = 'alertMessage alertMessageContent alertMessage-important';
			outputError.innerHTML = msg;
			errorElement.appendChild(outputError);

			var existsErrorElement = document.getElementById(this.existsErrorId);
			if (existsErrorElement != null && existsErrorElement != undefined) {
				var errorGroup = document.getElementById("errorGroupSurround");
				if (errorGroup != null && errorGroup != undefined) {
					if (errorGroup.className == "") {
						errorGroup.className = this.errorGroupClassName;
					}
				}
			
				var errorExistMessage = this.rule.submit();
				if (existsErrorElement.innerHTML.indexOf(errorExistMessage) < 0) {
					var outputExistsError = document.createElement('span');
					outputExistsError.className = 'alertMessageContentTitle';
					outputExistsError.innerHTML = errorExistMessage;
					existsErrorElement.innerHTML = outputExistsError.outerHTML;					
				}
			}			
		}
	},
	
	isExistSameErrorThisChildElements : function(errorElement, msg) {
		if (isNullorUndefined(errorElement)) {
			return false;
		}
		
		if (isNullorUndefined(errorElement.childNodes) || errorElement.childNodes.length == 0) {
			return false;
		}
		
		var childNodes = errorElement.childNodes;
		for (var i = 0; i < childNodes.length; i++) {
			var child = childNodes[i];
			if (isNullorUndefined(child) || isNullorUndefinedorEmpty(child.innerHTML)) {
				continue;
			}
			if (child.innerHTML.indexOf(msg) >= 0) {
				return true;
			}
		}
		return false;
	},
	
	getAlertMessageContents : function() {
		return $('.alertMessage');
	},
	
	isExistErrorMessage : function() {
		var alertMessageContents = this.getAlertMessageContents();
		if (alertMessageContents.length > 0) {
			return true;
		} else {
			return false;
		}
	},
	
	isExistErrorMessageWithScreenError : function() {
		var alertMessageContents = $('.alertMessageContent');
		if (alertMessageContents.length > 0) {
			return true;
		} else {
			return false;
		}
	},
	
	focusToFirstErrorElement : function() {
		var alertMessageContents = this.getAlertMessageContents();
		if (alertMessageContents.length == 0) {
			return false;
		}
		
		for (var i = 0; i < alertMessageContents.length; i++) {
			var alertMessageContent = alertMessageContents[0];
			var alertMessageContentParent = alertMessageContent.parentNode;
			if (isNullorUndefined(alertMessageContentParent)) {
				continue;
			}
			
			var alertMessageContentParentId = alertMessageContentParent.id;
			if (isNullorUndefinedorEmpty(alertMessageContentParentId)) {
				continue;
			}
			
			var targetFocusId = alertMessageContentParentId.replace(new RegExp("^" + this.errorId), "");
			focusToItem(targetFocusId);
			return true;
		}
		return false;		
	},
	
	tohalf : function(str) {
		if (str == null || str == undefined || str == "") {
			return "";
		}
		
		var half = str.replace(/[Ａ-Ｚａ-ｚ０-９]/g, function(s) {
			return String.fromCharCode(s.charCodeAt(0) - 0xFEE0);
		});
		
		half = half.replace(/．/g, ".");
		half = half.replace(/[ー－]/g, "-");
		half = half.replace(/：/g, ":");
		
		return half;
	}
};

Validator.rule = {
	msg : null,

	submit : function() {
		return this.msg.submit;
	},

	input : function() {
		if (this.field.type == 'radio' || this.field.type == 'checkbox') {
			for (var i = 0, e; ; i++) {
				e = this.field.form[this.field.name][i];
				if (e == null || e == undefined) {
					break;
				}
				if (e.checked) {
					return;
				}
			}
			// 単一チェックボックスの場合の判定
			var singleChecked = this.field.form[this.field.name].checked;
			if (singleChecked != null && singleChecked != undefined && singleChecked) {
				return;
			}
			return this.msg.noselect;
		} else if (this.value == '') {
			return (this.field.type == 'select-one' || this.field.type == 'select-multiple') ? this.msg.noselect
					: this.msg.noinput;
		}
	},

	mail : function() {
		this.tohalf();
		if (!this.value.match(/^(?:[\w\-+./~?#=&`{}_]+)@(?:[0-9A-Za-z]+(?:[-.][0-9A-Za-z]+)*\.[0-9A-Za-z]+(?:[-.][0-9A-Za-z]+)*)$/)) {
			return this.msg.mail;
		}
	},

	equal : function() {
		if (this.field.form[this.extra].value
				&& this.value != this.field.form[this.extra].value) {
			return this.msg.unequal;
		}
	},

	alphabet : function() {
		this.tohalf();
		if (!this.value.match(/^[a-zA-Z]+$/)) {
			return this.msg.alphabet;
		}
	},

	alphabet_with_camma : function() {
		this.tohalf();
		if (!this.value.match(/^[a-zA-Z,]+$/)) {
			return this.msg.alphabet;
		}
	},

	alphabetNumber : function() {
		this.tohalf();
		if (!this.value.match(/^[\x01-\x7F]+$/)) {
			return this.msg.alphabetNumber;
		}
	},

	kana : function(arg) {
		for (var i = 0; i < this.value.length; i++) {
			if (this.value.charAt(i) == ' ' || this.value.charAt(i) == '\u3000'){
				continue;
			}
			if (this.value.charAt(i) < '\u30A1'
					|| this.value.charAt(i) > '\u30F6') {
				return this.msg.kana.replace(/%1/g, arg);
			}
		}
	},

	zenkaku : function() {
		if (!this.value.match(/^[０-９Ａ-Ｚａ-ｚぁ-んァ-ヶ一-龠々－ー（） 　ヽヾゝゞ〆・]*$/)) {
			return this.msg.zenkaku;
		}
	},

	tel : function() {
		if (!this.value.match(/^\d{2,5}-\d{1,4}-\d{3,4}$/) || !this.value.match(/^[\d-]{12,13}$/)) {
			return this.msg.tel;
		}
	},
	
	fax : function() {
		if (!this.value.match(/^\d{2,5}-\d{1,4}-\d{3,4}$/) || !this.value.match(/^[\d-]{12,13}$/)) {
			return this.msg.fax;
		}
	},

	zip : function() {
		if (!this.value.match(/^[\d]{3}\-[\d]{4}$/)) {
			return this.msg.zip;
		}
	},

	password : function() {
		if (!this.value.match(/^[A-Za-z0-9`'\)|\+\{_=:\?\[@!\(\]\$>*-\}&<^;/~,"\.#%\\]*$/)) {
			return this.msg.password;
		}
	},

	count : function(arg) {
		return this._range(arg, this.value.length, this.msg.count);
	},

	num : function(arg) {
		this.tohalf();
		if (!this.value.match(/^[\d,\.-]+$/)) {
			return this.msg.num.nonumber;
		}
		// カンマを外す
		var number = this.value.replace(/,/g, "");
		return this._range(arg, parseFloat(number), this.msg.num);
	},

	numCount : function(arg) {
		// カンマを外す
		var number = this.value.replace(/,/g, "");
		// 小数点以下をスプリット
		number = number.split("\.")[0];
		// マイナスは桁数に数えない
		number = number.replace(/-/g, "");
		return this._range(arg, number.length, this.msg.numCount);
	},

	check : function(arg) {
		var value = 0;
		for ( var i = 0, e; e = this.field.form[this.field.name][i]; i++){
			if (e.checked) {
				value += 1;
			}
		}

		return this._range(arg, value, this.msg.check);
	},
	
	tohalf : function() {
		var half = Validator.tohalf(this.value);
		this.field.value = half;
		this.value = half;
	},

	_range : function(range, value, msg) {
		if (!range)
			return;

		var result = '';
		var c = (" " + range).split(/:/);

		var isSetMin = true;
		var min = parseFloat(c[0]);
		if (min.toString() == "NaN") {
			isSetMin = false;
		}

		var isSetMax = true;
		var max = parseFloat(c[1]);
		if (max.toString() == "NaN") {
			isSetMax = false;
		}
		if (isSetMin && value != min && /^\d+$/.test(range)) {
			result = msg.unequal;
		} else if (!isSetMin && value > max) {
			result = msg.too_big;
		} else if (!isSetMax && value < min) {
			result = msg.too_small;
		} else if (isSetMin && isSetMax && (value < min || value > max)) {
			result = msg.outofrange;
		}
		return result.replace(/%1/g, min).replace(/%2/g, max);
	}
};

Validator.lang = {
	ja : {
		noselect : '必須選択項目です。選択してください。',
		noinput : '必須入力項目です。入力してください。',
		unequal : '入力が揃っていません。',

		submit : 'エラーがあります。',
		mail : '正しい形式のメールアドレスを入力してください。',
		alphabet : '半角のアルファベットを入力してください。',
		alphabetNumber : '半角の英数文字を入力してください。',
		kana : '%1' + 'は全角カタカナで入力してください。',
		zenkaku : '全角で入力してください。',
		// tel : '正しい形式の電話番号を入力してください。',
		tel: '電話番号はハイフンを含め、12または13桁で入力してください。',
		fax: 'FAX番号はハイフンを含め、12または13桁で入力してください。',
		zip : '正しい形式の郵便番号を入力してください。',
		password : '正しい形式のパスワードを入力してください。',
		birthday : '有効な生年月日を入力してください。',

		count : {
			unequal : '%1' + '文字で入力してください。',
			too_big : '%2' + '文字以下で入力してください。',
			too_small : '%1' + '文字以上入力してください。',
			outofrange : '%1' + '文字以上' + '%2' + '文字以下で入力してください。'
		},

		num : {
			nonumber : '数値を入力してください。',
			unequal : '%1' + 'と入力してください。',
			too_big : '%2' + '以下の値を入力してください。',
			too_small : '%1' + '以上の値を入力してください。',
			outofrange : '%1' + 'から' + '%2' + 'の間で入力してください。'
		},

		numCount : {
			unequal : '%1' + '桁の値を入力してください。',
			too_big : '%2' + '桁以下の値を入力してください。',
			too_small : '%1' + '桁以上の値を入力してください。',
			outofrange : '%1' + '桁以上' + '%2' + '桁以下で入力してください。'
		},

		check : {
			unequal : 'チェックは' + '%1' + '個してください。',
			too_big : 'チェックは' + '%2' + '個までです。',
			too_small : 'チェックは' + '%1' + '個以上してください。',
			outofrange : 'チェックは' + '%1' + '個から' + '%2' + '個までです。'
		}
	}
};

Validator.rule.msg = Validator.lang.ja;
