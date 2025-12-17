/* 初期フォーカス無効化 */
isInitFocus = false;

var maxGoodsBuyMaxCount = 99999;

// 価格の末尾の0を削除するjsコード。
$(document).ready( function(){
	$('.removeZeroPrice').each(function(){
		if ($(this).html().indexOf('.') > 0) {
			$(this).html(replaceZero($(this).html()));
		}
	});
	$('.digiReelIcon').attr('src', getUrlWithContextPath('contents/shop/marutsu/img/icon/digiReelIcon.png'));
});

// 引数で与えられた文字列の末尾の0を削除するfunction
function replaceZero(target) {
	target = target.replace(/0+$/, '');
	target = target.replace(/\.$/, '');
	return target;
}

// 引数で与えられた数値の小数点以下を小さくするfunction
function makeSmallerBelowDecimalPoint(target) {
	var splitTarget = target.split('.');
	return splitTarget[0] + '<small>.' + splitTarget[1] + ' </small>';
}

//jQueryでスムース スクロール。
$(function(){
	$('p.pgtop a[href^=#]').click(function(){
		var speed = 500;
		var href= $(this).attr("href");
		var target = $(href == "#" || href == "" ? 'html' : href);
		var position = target.offset().top;
		$("html, body").animate({scrollTop:position}, speed, "swing");
		return false;
	});
});

//うっすらテキストを表示するjsコード。このまま貼るだけでＯＫ
$(document).ready(function() {
	if (0 < $('form input.help, form textarea.help').size()) {
    $('form input.help, form textarea.help').formtips({
        tippedClass: 'tipped'
    });
    }
});



//tabarea
$(function(){
	$(".tabarea .tabs li a").click(function(){
		var tab = $(this).parent().attr("id"); //クリックされたタブのidを取得
		var num = tab.split("_"); //セパレータで分割
		var con = "#content_" + num[1]; //表示させるコンテンツのid名を生成
		if (tab == 'tab_1' && !isGuest()) {
			if (!confirm("こちらのタブを使用するとログアウトされ新規会員として注文を行います。よろしいですか？")) {
				return false;
			} else {
				document.getElementById("guestFlag").value = "1";
				onclickGuestTabTitle(this, "logout");
			}
		} else if (tab == 'tab_3') {
			document.getElementById("guestFlag").value = "0";
		}
		$(this).parent().addClass("selected"); //クリックされたタブのclassにselectedを割り当て
		$(".tabarea li:not(#"+tab+")").removeClass("selected"); //その他のタブのclassにあるselectedを削除
		$(".tabarea div[id^=content]").hide(); //コンテンツをすべて非表示に
		$(".tabarea " + con).show(); //クリックされたタブ対応のコンテンツのみ表示
		return false;
	});

	priceRangeSetting();
	minOrderNumRangeSetting();
});

// 価格スライダーの設定
function priceRangeSetting() {
	sliderSetting("price", 0, 50000, 1000);
}

// 最低購入数量スライダーの設定
function minOrderNumRangeSetting() {
	sliderSetting("min_order_num", 0, 1000, 1);
}

// スライダーの設定
function sliderSetting(itemName, minSettingDefault, maxSettingDefault, stepDefault) {
	var slider = document.getElementById(itemName + "_range_slider");
	if (!isNullorUndefined(slider)) {

		var maxSetting = maxSettingDefault;
		var maxSettingHidden = getElementValueNumber(itemName + "_srider_max_value");
		if (!isNaN(maxSettingHidden)) {
			maxSetting = maxSettingHidden;
		}

		var minSetting = minSettingDefault;
		var minSettingHidden = getElementValueNumber(itemName + "_srider_min_value");
		if (!isNaN(minSettingHidden)) {
			minSetting = minSettingHidden;
		}

		var stepSetting = stepDefault;
		var stepSettingHidden = getElementValueNumber(itemName + "_srider_step_value");
		if (!isNaN(stepSettingHidden)) {
			stepSetting = stepSettingHidden;
		}


		var high = getElementValueNumber(itemName + "High");
		var highSetting;
		if (!isNaN(high)) {
			if (low > high) {
				low = high;
			}

			if (high > maxSetting) {
				high = maxSetting;
			}
			highSetting = getFormatedNumberString(high, 0, true);
		} else {
			highSetting = "";
			high = maxSetting + stepSetting;
		}

		var low = getElementValueNumber(itemName + "Low");
		var lowSetting;
		if (!isNaN(low)) {
			lowSetting = getFormatedNumberString(low, 0, true)
		} else {
			lowSetting = "";
			low = 0;
		}

		setValue(itemName + "_range_low", lowSetting, true);
		setValue(itemName + "_range_high", highSetting, true);

		$("#" + itemName + "_range_slider").slider({
			range: true,
			min: minSetting,
			max: maxSetting + stepSetting,
			values: [low, high],
			step: stepSetting,
			slide: function(event, ui) {
				var l = ui.values[0];
				var h = ui.values[1];

				if (l == 0) {
					setValue(itemName + "_range_low", "", true);
					setValue(itemName + "Low", "", true);
				} else {
					setValue(itemName + "_range_low", getFormatedNumberString(l, 0, true), true);
					setValue(itemName + "Low", l, true);
				}

				if (h == maxSetting + stepSetting) {
					setValue(itemName + "_range_high", "", true);
					setValue(itemName + "High", "", true);
				} else {
					setValue(itemName + "_range_high", getFormatedNumberString(h, 0, true), true);
					setValue(itemName + "High", h, true);
				}
			}
	    });
	}
}

function submitPrice() {
	submitSlider("price");
}

function submitMinOrderNum() {
	submitSlider("min_order_num");
}

function submitSlider(itemName) {
	setSliderHidden("price");
	setSliderHidden("min_order_num");
	submit(itemName);
}

function setSliderHidden(itemName) {
	var low = getElementValueNumber(itemName + "_range_low");
	if (isNaN(low)) {
		low = "";
	}
	setValue(itemName + "Low", low, true);
	var high = getElementValueNumber(itemName + "_range_high");
	if (isNaN(high)) {
		high = "";
	}
	setValue(itemName + "High", high, true);
}

// 会員情報変更、ネット会員登録で使用します。
function switchMailSendBanner() {
	var yes = $('[name="mailMagazineFlg"]:checked')[0].value == "1";
	//var no = document.getElementById("mailMagazineFlg2").checked;
	if (yes) {
		//document.getElementById('mailBannerArea').style.display = 'none';
		$('.mailBannerArea').hide();
	} else {
	//if (no) {
		//document.getElementById('mailBannerArea').style.display = 'block';
		$('.mailBannerArea').show();
	}
}

function jumpTo(arg){
	location.href =  "#"+ arg;
}

//---プリロード用のfunction
function preloadFunc() {
	for(var i = 0; i<arguments.length; i++){
		$("<img>").attr("src", arguments[i]);
	}
}

$(function () {
	preloadFunc(
/*		"contents/img/common/header/gnavi_01_ov.png",
		"contents/img/common/header/gnavi_01_on.png",
		"contents/img/common/header/gnavi_02_ov.png",
		"contents/img/common/header/gnavi_02_on.png",
		"contents/img/common/header/gnavi_03_ov.png",
		"contents/img/common/header/gnavi_03_on.png",
		"contents/img/common/header/gnavi_04_ov.png",
		"contents/img/common/header/gnavi_04_on.png",
		"contents/img/common/header/gnavi_05_ov.png",
		"contents/img/common/header/gnavi_05_on.png",
		"contents/img/common/header/btn_mypage_ov.png",
		"contents/img/common/header/btn_cart_ov.png",
		"contents/img/common/header/hnavi_01_ov.png",
		"contents/img/common/header/hnavi_01_on.png",
		"contents/img/common/header/hnavi_02_ov.png",
		"contents/img/common/header/hnavi_02_on.png",
		"contents/img/common/header/fc_mdl_ov.png",
		"contents/img/common/header/fc_mdl_on.png",
		"contents/img/common/header/fc_lag_ov.png",
		"contents/img/common/header/fc_lag_on.png",
		"contents/img/common/header/sp/gnavi_01_on.png",
		"contents/img/common/header/sp/gnavi_02_on.png",
		"contents/img/common/header/sp/gnavi_03_on.png",
		"contents/img/common/header/sp/gnavi_04_on.png"
*/
	);
});

//マイページメニュー（SP）
$(function(){
	var $menubox = $("#leftContents .memberMenuBox");
	var $mbtitle = $menubox.find(".memberMenuBox_tit");
	var $mbmenu = $menubox.find("ul");

	if($menubox.length){
		$mbmenu.is(":visible") ? $mbtitle.removeClass("close") : $mbtitle.addClass("close");
		$mbtitle.click(function(){
			$mbmenu.slideToggle("fast",function(){
				$(this).is(":visible") ? $mbtitle.removeClass("close") : $mbtitle.addClass("close");
			});
		});
	}
});

//$(function() {
// $('input, textarea').placeholder();
//});


/* tile */
(function($) {
	$.fn.tile = function(columns) {
		var tiles, max, c, h, last = this.length - 1, s;
		if(!columns) columns = this.length;
		this.each(function() {
			s = this.style;
			if(s.removeProperty) s.removeProperty("height");
			else if(s.removeAttribute) s.removeAttribute("height");
		});
		return this.each(function(i) {
			c = i % columns;
			if(c == 0) tiles = [];
			tiles[c] = $(this);
			h = tiles[c].height();
			if(c == 0 || h > max) max = h;
			if(i == last || c == columns - 1)
				$.each(tiles, function() { this.height(max); });
		});
	};
})(jQuery);


/*
 * fixHeight - jQuery Plugin
 * http://www.starryworks.co.jp/blog/tips/javascript/fixheightjs.html
 *
 * Author Koji Kimura @ STARRYWORKS inc.
 * http://www.starryworks.co.jp/
 *
 * Licensed under the MIT License
 *
 */
(function($){

	var isInitialized = false;
	var parents = [];
	var textHeight = 0;
	var $fontSizeDiv;

	$.fn.fixHeight = function() {
		this.each(function(){
			var childrenGroups = getChildren( this );

			$.each( childrenGroups, function(){

				var $children = $(this);
				if ( !$children.filter(":visible").length ) return;

				var row = [];
				var top = 0;
				$children.each(function(){
					if ( top != $(this).position().top ) {
						$(row).sameHeight();
						row = [];
						top = $(this).position().top;
					}
					row.push(this);
				});
				if ( row.length ) $(row).sameHeight();
			});


		});
		init();
		return this;
	}

	$.checkFixHeight = function( i_force ) {
		if ( $fontSizeDiv.height() == textHeight && i_force !== true ) return;
		textHeight = $fontSizeDiv.height();
		$(parents).fixHeight();
	}

	$.fn.sameHeight = function() {
		var maxHeight = 0;
		this.css("height","auto");
		this.each(function(){
			if ( $(this).height() > maxHeight ) maxHeight = $(this).height();
		});
		return this.height(maxHeight);
	}

	function getChildren( i_parent ) {
		var $parent = $( i_parent );

		if ( $parent.data("fixHeightChildrenGroups") ) return $parent.data("fixHeightChildrenGroups");
		var childrenGroups = [];

		var $children = $parent.find(".fixHeightChild");
		if ( $children.length ) childrenGroups.push( $children );

		var $groupedChildren = $parent.find("*[class*='fixHeightChild']:not(.fixHeightChild)");
		if ( $groupedChildren.length ) {
			var classNames = {};
			$groupedChildren.each(function(){
				var a = $(this).attr("class").split(" ");
				var i;
				var l = a.length;
				var c;
				for ( i=0; i<l; i++ ) {
					c = a[i].match(/fixHeightChild[a-z0-9_-]+/i);
					if ( !c ) continue;
					c = c.toString();
					if ( c ) classNames[c] = c;
				}
			});
			for ( var c in classNames ) childrenGroups.push( $parent.find("."+c) );
		}

		if ( !childrenGroups.length ) {
			$children = $parent.children();
			if ( $children.length ) childrenGroups.push( $children );
		}

		$parent.data("fixHeightChildrenGroups", childrenGroups );
		parents.push( $parent );

		return childrenGroups;
	}


	function init() {
		if ( isInitialized ) return;
		isInitialized = true;
		$fontSizeDiv = $(document).append('<div style="position:absolute;left:-9999px;top:-9999px;">s</div>');
		setInterval($.checkFixHeight,1000);
		$(window).resize($.checkFixHeight);
		$.checkFixHeight();
		$(window).load( function(){ $.checkFixHeight(true); } );
	}

})(jQuery);


jQuery(document).ready(function(){
	$(".fixHeight").fixHeight();
});

$(function(){
		$(".lineupbox").fixHeight();
});

$(function (){
	if($(".noticebtn.forSp").length){
	$.modal.defaults.closeClass = "modalClose";
	$(".noticebtn.forSp").click(function (e) {
			var _pos_top = $(".noticebtn.forSp").offset().top;
			var _pos_left = ($(window).width() - 300) / 2;
			$('.inquirybefore').modal({
				overlayClose:true,
				onShow: function (d) {
				d.container.css({position: 'absolute',top:_pos_top,left:_pos_left,height:600});
			},
				autoPosition : false,
				onClose: function(d){
					$.modal.close();
					$("html,body").animate({scrollTop:_pos_top},500);
				}
			});
			$("html,body").animate({scrollTop:_pos_top},500);
		return false;
	});
	if($('.inquirybefore').length){
		$(window).resize(function(){
			if($(window).width() > 768){
				$.modal.close();
			}
		});
	}
	}
});

$(function (){
	var _switch = $("a.pop_privacy");
	if(_switch.length){
	$.modal.defaults.closeClass = "modalClose";
	_switch.click(function (e) {
			var _pos_top = _switch.offset().top;
			var _pos_left = ($(window).width() - 300) / 2;
			$('div.privacypolicy').modal({
				overlayClose:true,
				onShow: function (d) {
				d.container.css({position: 'absolute',top:_pos_top,left:_pos_left,height:400});
				android_v2_scroll();
			},
				autoPosition : false,
				onClose: function(d){
					$.modal.close();
					$("html,body").animate({scrollTop:_pos_top},500);
				}
			});
			$("html,body").animate({scrollTop:_pos_top},500);
		return false;
	});
	if($('div.privacypolicy').length){
		$(window).resize(function(){
			if($(window).width() > 768){
				$.modal.close();
			}
		});
	}
	}
});

function android_v2_scroll(){
	var ua = navigator.userAgent.toLowerCase();
	var version = ua.substr(ua.indexOf('android')+8, 3);
	if(ua.indexOf("android")){
		if(parseFloat(version) <= 2.3) $('.simplemodal-wrap').flickable();
	}

}

$(window).load(function(){

	//オブジェクト情報取得
	var $container = $(".contents");
	if($container.length == 0){
		$container = $(".contents_full");
	}
	var $side = $("#rightContents");
	var $sidebox = $("#rightContents .memberMenuBox");
	var $pgtop = $(".pgtop");
	var _container_offset_top = parseInt($container.offset().top);
	var _start_delay = 0;
	if($(".contents > .mem_lead").length){
		_start_delay =  parseInt($(".contents > .mem_lead").offset().top);
	}

	if($sidebox.length && $sidebox.outerHeight() < ($container.outerHeight() - $pgtop.outerHeight() - 100)){
	$(window).scroll(function () {
		var ScrTop = $(document).scrollTop();
		var _sidebox_height = parseInt($sidebox.outerHeight());
		var _container_height = parseInt(_container_offset_top + $container.outerHeight());

		//左メニュー固定切替
		if(ScrTop > (_container_offset_top + _start_delay)){
			$sidebox.removeClass("bottom").addClass("fixed");
		}

		//左メニュー固定 #container下辺まで
		if(ScrTop > (_container_height - _sidebox_height - $pgtop.outerHeight())){
			$sidebox.removeClass("fixed").addClass("bottom");
		}

		//左メニュー通常状態
		if(ScrTop < (_container_offset_top + _start_delay)){
			$sidebox.removeClass("fixed");
		}

	});

	}

});

/**
 * 全体のプレースホルダーの表示非表示を制御します。
 */
function cleanPlaceholder() {
	if (jQuery.browser.msie) {
		var inputElements = $('input');
		inputElements.each(function(idx) {
			if ($(this).attr("placeholder")) {
				$(this).triggerHandler("change");
			}
		});
	}
}

/**
 * 定格項目の件数を取得するAjax処理です。
 * 呼び出し後、callbackRatedCountメソッドが呼び出されるので、そちらで件数の処理を実施してください。
 */
function rvAjax() {
    var goodsClassCode1 = getElementValue("goodsClassCode1");
    var goodsClassCode2 = getElementValue("goodsClassCode2");
    var goodsClassCode3 = getElementValue("goodsClassCode3");
    var sk = getElementValue("searchKeyword");
	var keysSize = getDataSize("alreadySearch");
	var paramKeys = null;
	if (keysSize != 0) {
		var paramKeys = new Array();
		for (var i = 0; i < keysSize; i++) {
			var keyValue = getItemValueFromList("alreadySearch", "key", i);
			if (!isNullorUndefinedorEmpty(keyValue)) {
				paramKeys.push(keyValue);
			}
		}
	}


    var paramMaker = getMultiSelectValues("maker");
    var paramSn = getMultiSelectValues("sn");
    var paramDeliDate = getMultiSelectValues("deliDate");

    var rohs = document.getElementById("rohs");
    var paramRohs = null;
    if (!isNullorUndefined(rohs) && rohs.checked) {
    	paramRohs = "1";
    }

    var spice = document.getElementById("spice");
    var paramSpice = null;
    if (!isNullorUndefined(spice) && spice.checked) {
    	paramSpice = "1";
    }

    var tds = document.getElementById("tds");
    var paramTds = null;
    if (!isNullorUndefined(tds) && tds.checked) {
    	paramTds = "1";
    }
    var paramRv1 = getMultiSelectValues("rv1");
    var paramRv2 = getMultiSelectValues("rv2");
    var paramRv3 = getMultiSelectValues("rv3");
    var paramRv4 = getMultiSelectValues("rv4");
    var paramRv5 = getMultiSelectValues("rv5");
    var paramRv6 = getMultiSelectValues("rv6");
    var paramRv7 = getMultiSelectValues("rv7");
    var paramRv8 = getMultiSelectValues("rv8");
    var paramRv9 = getMultiSelectValues("rv9");
    var paramRv10 = getMultiSelectValues("rv10");
    var paramRv11 = getMultiSelectValues("rv11");
    var paramRv12 = getMultiSelectValues("rv12");
    var paramRv13 = getMultiSelectValues("rv13");
    var paramRv14 = getMultiSelectValues("rv14");
    var paramRv15 = getMultiSelectValues("rv15");
    var paramRv16 = getMultiSelectValues("rv16");
    var paramRv17 = getMultiSelectValues("rv17");
    var paramRv18 = getMultiSelectValues("rv18");
    var paramRv19 = getMultiSelectValues("rv19");
    var paramRv20 = getMultiSelectValues("rv20");

    // 即納在庫絞込対応
    var idsf = $('form[name=formMain]').find('#idsf').val();
    var ids = "";
    if ($('form[name=formMain]').find('#ids').prop("checked")) {
    	ids = "1";
    }

    // 1個から購入可能商品のみで表示
    var sdgf = $('form[name=formMain]').find('#sdgf').val();
    var sdg = "";
    if ($('form[name=formMain]').find('#sdg').prop("checked")) {
    	sdg = "1";
    }

    $.ajaxSetup({ cache: false });
    var ajaxUrl = getUrlWithContextPath("jp/eltex/xsp/ajax/custom/AjaxRatedCountBean.json", "");
    $.getJSON(ajaxUrl, {"goodsClassCode1":goodsClassCode1, "goodsClassCode2":goodsClassCode2, "goodsClassCode3":goodsClassCode3, "keys":paramKeys, "sk":sk, "maker":paramMaker,　"deliDate":paramDeliDate, "sn":paramSn, "rohs":paramRohs, "spice":paramSpice, "tds":paramTds, "rv1":paramRv1, "rv2":paramRv2, "rv3":paramRv3, "rv4":paramRv4, "rv5":paramRv5, "rv6":paramRv6, "rv7":paramRv7, "rv8":paramRv8, "rv9":paramRv9, "rv10":paramRv10, "rv11":paramRv11, "rv12":paramRv12, "rv13":paramRv13, "rv14":paramRv14, "rv15":paramRv15, "rv16":paramRv16, "rv17":paramRv17, "rv18":paramRv18, "rv19":paramRv19, "rv20":paramRv20, "ids":ids, "idsf":idsf, "sdg":sdg, "sdgf":sdgf}, function(info) {
        if (isNullorUndefinedorEmpty(info) || isNullorUndefinedorEmpty(info.results)) {
            return;
        }

        // Callback 処理
        var totalCount = 0;
        if (!isNullorUndefinedorEmpty(info.results.totalCount)) {
        	totalCount = info.results.totalCount;
        }

        // メーカー名
        var makerSelections = "";
        if (!isNullorUndefinedorEmpty(info.results.makerSelections)) {
        	makerSelections = info.results.makerSelections;
        }

        // 納期
        var deliDateSelections = "";
        if (!isNullorUndefinedorEmpty(info.results.deliDateSelections)) {
        	deliDateSelections = info.results.deliDateSelections;
        }

        // 在庫拠点
		var snSelections = "";
        if (!isNullorUndefinedorEmpty(info.results.snSelections)) {
        	snSelections = info.results.snSelections;
        }

        // 定格項目
        var sortedRatedCountInfo = "";
        if (!isNullorUndefinedorEmpty(info.results.sortedRatedInfo)) {
        	sortedRatedInfo = info.results.sortedRatedInfo;
        }

        callbackRatedCount(totalCount, makerSelections, deliDateSelections, snSelections, sortedRatedInfo);
    });
}

/**
 * 待機表示中のダイアログを表示します。[eltex.standard.jsの上書き]
 */
function showWaitDialog() {
	var waitDialog = getWaitDialog();
	if (!isNullorUndefined(waitDialog)) {
		waitDialog.style.display = "block";
		var documentWidth = $(document).width();
		var documentHeight = $(document).height();
		$("#wait_dialog").css("width", documentWidth);
		$("#wait_dialog").css("height", documentHeight);

	    var left = Math.floor(($(window).width() - $("#wait_dialog_message").width()) / 2);
	    var top  = Math.floor(($(window).height() - $("#wait_dialog_message").height()) / 2);
	    $("#wait_dialog_message").css({
            "top": top,
            "left": left
         })
	}
}

// ページトップスクロール
$(function ($) {
	var tab = $('#side'),
			offset = tab.offset();

	$(window).scroll(function () {
		if ($(window).scrollTop() > offset.top) {
			tab.addClass('fixed');
		} else {
			tab.removeClass('fixed');
		}
	});
});

$(function () {
	var showTop = 100;
	var fixedTop = $('#side');
	$(window).on('load scroll resize', function () {
		if ($(window).scrollTop() >= showTop) {
			fixedTop.fadeIn('normal');
		} else if ($(window).scrollTop() < showTop) {
			fixedTop.fadeOut('normal');
		}
	});
});

// ページトップスクロール
//スマホ対応版
$(function ($) {
	var tab = $('#side'),
			offset = tab.offset();
	var body = $('body');
	body.scroll(function () {
		var top = offset.top - 300;
		if (body.scrollTop() > top) {
			tab.addClass('fixed');
			tab.fadeIn('normal');
			document.getElementById('side').style.display = 'block';
			document.getElementById('side').style.bottom = '250px';
		} else {
			tab.removeClass('fixed');
			tab.fadeOut('normal');
			document.getElementById('side').style.display = 'none';
		}
	});
});

// 右スライダーリンク スクロールで表示
$(function () {
	var fixedTop = $('#slideR');
	//初期表示時に閉じた状態とする//
	fixedTop.addClass('off');
	fixedTop.animate({'marginRight':'0px'},400);
	fixedTop.removeClass('on');
	fixedTop.fadeIn('normal');
});

$(function () {
$(".shopping_cart").on("mouseover", function() {
		var minWidth = 440;
		if ($(window).width() < minWidth) {
	    	return false;
	    }
        if (!$(this).next().hasClass("active")){
            $(this).next().slideToggle();
    		$(this).next().toggleClass("active");
    	}
 });


$(document).on('mouseover', function(event) {
  if (!$(event.target).closest("#main_header #topGroupName").length) {
    if ($(event.target).closest(".shopping_cart").length) {
    	return false;
    }
    var minWidth = 440;
	if ($(window).width() < minWidth) {
    	return false;
    }
    if ($("#main_header #topGroupName").hasClass("active")){
 			$("#main_header #topGroupName").slideToggle();
    		$("#main_header #topGroupName").toggleClass("active");
 		}
  }
 });
});

/**
 * 共通関数
 * Cookieからキーに紐づく値を取得
 *
 * @param {取得したいクッキーのキー名} key
 * @param {取得したいクッキーの値 ※この値を無しにすると無条件でクッキーの値を取得可能} val
 * @returns {}
 */
function getCookieValue(key, val){
	//戻り値
	var cValue = null;
	var cookie = document.cookie.split(';');
	cookie.forEach(function(value) {
	    //cookie名と値に分ける
	    var content = value.split('=');
	    //キーがあるか検索
	    if (content[0] == key) {
	    	//値が正常かチェック
	    	if (content[1] == val) {
				cValue = content[1];
			}
			else{
				//正常でないが、値の引数がブランクの場合は値をそのまま戻す
				if (val == "") {
					cValue = content[1];
				}
			}
		}
	})
	return cValue;
}

/** カート保存不可制御の共通定数1 */
const STR_NOT_ADD_CART_1 = 'カートに保存できませんでした。';
/** カート保存不可制御の共通定数2 */
const STR_NOT_ADD_CART_2 = '以下の場合が考えられます。';
/** カート保存不可制御の共通定数3 */
const STR_NOT_ADD_CART_3 = '・カートに保存できる種類数を超過しています。';
/** カート保存不可制御の共通定数4 */
const STR_NOT_ADD_CART_4 = '　※カートに保存できる商品は';
/** カート保存不可制御の共通定数5 */
const STR_NOT_ADD_CART_5 = '種類までです';
/** カート保存不可制御の共通定数6 */
const STR_NOT_ADD_CART_6 = '・カートに保存できない商品を保存しようとしました。';

/**
 * べんりねっと対応
 *    ＆
 * 物品調達システム対応※パナソニックのみ
 *    &
 * カウネット対応
 * カート保存不可共通関数
 * @param {} uLightBox
 * @returns {} ret
 */
function commonNotAddCart(uLightBox){
	//戻り値
	var ret = false;
	//Cookieから指定した値の取得
	var selectItemMax = getCookieValue(" selectItemMax", "");
	var notAddCart = getCookieValue(" notAddCart", "true");
	//カート保存不可Cookieの存在有無で表示するポップアップを変える
	if (notAddCart == "true") {
		if (selectItemMax != "0") {
			//カート追加無しCookieは参照したら一回ずつ削除する（最大件数が0出ない場合のみ）
			document.cookie = "notAddCart=; max-age=0";
		}
		//存在有り
		addCartTitle = STR_NOT_ADD_CART_1;
		addCartMsg = "<br>"
		           + "<center><p><strong>" + STR_NOT_ADD_CART_2 + "</strong></center></p>"
		           + "<left><p><strong>" + STR_NOT_ADD_CART_3 + "</strong></left></p>"
		           + "<left><p><strong>"  + STR_NOT_ADD_CART_4 + selectItemMax + STR_NOT_ADD_CART_5 + "</strong></left></p>"
		           + "<left><p><strong>" + STR_NOT_ADD_CART_6 + "</strong></left></p>";

		if (checkKauNetMode()) {
			// カウネットの場合はデジリール手数料が商品毎に1明細とカウントされる
			// 最大件数との誤差が発生するためメッセージを切り替える（件数を表示しない）
			addCartMsg = "<br>"
		           + "<center><p><strong>" + STR_NOT_ADD_CART_2 + "</strong></center></p>"
		           + "<left><p><strong>" + STR_NOT_ADD_CART_3 + "</strong></left></p>"
		           + "<left><p><strong>" + STR_NOT_ADD_CART_6 + "</strong></left></p>";
		}

		//uLightBox初期化
		uLightBox.init({
			override:true,
			background: 'white',
			centerOnResize: true,
			fade: false
		});
		//uLightBox表示
		uLightBox.alert({
		width: '300px',
		title: addCartTitle,
		text: addCartMsg,
		leftButtons: ['お買い物を続ける'],
		rightButtons: ['決済に進む'],
		onClick: function(button) {
			if(button == '決済に進む') {
				location.href =  '/Cart.jsp';
			} else {
				uLightBox.clear();
			}
		}});
		ret = true;
	}
	return ret;
}

/**
 * パンチアウトモード判定
 * @returns {}
 */
function checkPunchOutMode(){
	var ret = false;
	var isPunchOutModeElement = document.getElementById('isPunchOutMode');
	if (isPunchOutModeElement != null && isPunchOutModeElement != undefined) {
		if (isPunchOutModeElement.value == 'true') {
			ret = true;
		}
	}
	return ret;
}

/**
 * 物品調達システム対応
 * 物品調達システムタイプを判定
 * @returns {}
 */
function checkGoodsProcurementType(type){
	//戻り値
	var ret = false;
	//タイプ値取得
	var goodsProcurementType = document.getElementById('goodsProcurementType');
	if (goodsProcurementType != null && goodsProcurementType != undefined) {
		//タイプが存在すれば判定
		if (goodsProcurementType.value == type) {
			ret = true;
		}
	}
	return ret;
}

/**
 * カウネット対応：カウネットモード判定
 * @returns {Boolean}
 */
function checkKauNetMode() {

	var ret = false;
	var isKauNetModeElement = document.getElementById('isKauNetMode');
	if (isKauNetModeElement != null && isKauNetModeElement != undefined) {
		if (isKauNetModeElement.value == 'true') {
			ret = true;
		}
	}
	return ret;
}

/**
 * 電話番号を整形する
 */
var getFormatPhone = function($INPUT, $STRICT){
	$STRICT = $STRICT || false;
	// 市外局番のグループ定義
	// データは http://www.soumu.go.jp/main_sosiki/joho_tsusin/top/tel_number/number_shitei.html より入手後、整形
	var group = {
		5 : {
			"01267" : 1, "01372" : 1, "01374" : 1, "01377" : 1, "01392" : 1, "01397" : 1, "01398" : 1, "01456" : 1, "01457" : 1, "01466" : 1, "01547" : 1,
			"01558" : 1, "01564" : 1, "01586" : 1, "01587" : 1, "01632" : 1, "01634" : 1, "01635" : 1, "01648" : 1, "01654" : 1, "01655" : 1, "01656" : 1,
			"01658" : 1, "04992" : 1, "04994" : 1, "04996" : 1, "04998" : 1, "05769" : 1, "05979" : 1, "07468" : 1, "08387" : 1, "08388" : 1, "08396" : 1,
			"08477" : 1, "08512" : 1, "08514" : 1, "09496" : 1, "09802" : 1, "09912" : 1, "09913" : 1, "09969" : 1,
		},
		4 : {
			"0123" : 2, "0124" : 2, "0125" : 2, "0126" : 2, "0133" : 2, "0134" : 2, "0135" : 2, "0136" : 2, "0137" : 2, "0138" : 2, "0139" : 2, "0142" : 2,
			"0143" : 2, "0144" : 2, "0145" : 2, "0146" : 2, "0152" : 2, "0153" : 2, "0154" : 2, "0155" : 2, "0156" : 2, "0157" : 2, "0158" : 2, "0162" : 2,
			"0163" : 2, "0164" : 2, "0165" : 2, "0166" : 2, "0167" : 2, "0172" : 2, "0173" : 2, "0174" : 2, "0175" : 2, "0176" : 2, "0178" : 2, "0179" : 2,
			"0182" : 2, "0183" : 2, "0184" : 2, "0185" : 2, "0186" : 2, "0187" : 2, "0191" : 2, "0192" : 2, "0193" : 2, "0194" : 2, "0195" : 2, "0197" : 2,
			"0198" : 2, "0220" : 2, "0223" : 2, "0224" : 2, "0225" : 2, "0226" : 2, "0228" : 2, "0229" : 2, "0233" : 2, "0234" : 2, "0235" : 2, "0237" : 2,
			"0238" : 2, "0240" : 2, "0241" : 2, "0242" : 2, "0243" : 2, "0244" : 2, "0246" : 2, "0247" : 2, "0248" : 2, "0250" : 2, "0254" : 2, "0255" : 2,
			"0256" : 2, "0257" : 2, "0258" : 2, "0259" : 2, "0260" : 2, "0261" : 2, "0263" : 2, "0264" : 2, "0265" : 2, "0266" : 2, "0267" : 2, "0268" : 2,
			"0269" : 2, "0270" : 2, "0274" : 2, "0276" : 2, "0277" : 2, "0278" : 2, "0279" : 2, "0280" : 2, "0282" : 2, "0283" : 2, "0284" : 2, "0285" : 2,
			"0287" : 2, "0288" : 2, "0289" : 2, "0291" : 2, "0293" : 2, "0294" : 2, "0295" : 2, "0296" : 2, "0297" : 2, "0299" : 2, "0422" : 2, "0428" : 2,
			"0436" : 2, "0438" : 2, "0439" : 2, "0460" : 2, "0463" : 2, "0465" : 2, "0466" : 2, "0467" : 2, "0470" : 2, "0475" : 2, "0476" : 2, "0478" : 2,
			"0479" : 2, "0480" : 2, "0493" : 2, "0494" : 2, "0495" : 2, "0531" : 2, "0532" : 2, "0533" : 2, "0536" : 2, "0537" : 2, "0538" : 2, "0539" : 2,
			"0544" : 2, "0545" : 2, "0547" : 2, "0548" : 2, "0550" : 2, "0551" : 2, "0553" : 2, "0554" : 2, "0555" : 2, "0556" : 2, "0557" : 2, "0558" : 2,
			"0561" : 2, "0562" : 2, "0563" : 2, "0564" : 2, "0565" : 2, "0566" : 2, "0567" : 2, "0568" : 2, "0569" : 2, "0572" : 2, "0573" : 2, "0574" : 2,
			"0575" : 2, "0576" : 2, "0577" : 2, "0578" : 2, "0581" : 2, "0584" : 2, "0585" : 2, "0586" : 2, "0587" : 2, "0594" : 2, "0595" : 2, "0596" : 2,
			"0597" : 2, "0598" : 2, "0599" : 2, "0721" : 2, "0725" : 2, "0735" : 2, "0736" : 2, "0737" : 2, "0738" : 2, "0739" : 2, "0740" : 2, "0742" : 2,
			"0743" : 2, "0744" : 2, "0745" : 2, "0746" : 2, "0747" : 2, "0748" : 2, "0749" : 2, "0761" : 2, "0763" : 2, "0765" : 2, "0766" : 2, "0767" : 2,
			"0768" : 2, "0770" : 2, "0771" : 2, "0772" : 2, "0773" : 2, "0774" : 2, "0776" : 2, "0778" : 2, "0779" : 2, "0790" : 2, "0791" : 2, "0794" : 2,
			"0795" : 2, "0796" : 2, "0797" : 2, "0798" : 2, "0799" : 2, "0820" : 2, "0823" : 2, "0824" : 2, "0826" : 2, "0827" : 2, "0829" : 2, "0833" : 2,
			"0834" : 2, "0835" : 2, "0836" : 2, "0837" : 2, "0838" : 2, "0845" : 2, "0846" : 2, "0847" : 2, "0848" : 2, "0852" : 2, "0853" : 2, "0854" : 2,
			"0855" : 2, "0856" : 2, "0857" : 2, "0858" : 2, "0859" : 2, "0863" : 2, "0865" : 2, "0866" : 2, "0867" : 2, "0868" : 2, "0869" : 2, "0875" : 2,
			"0877" : 2, "0879" : 2, "0880" : 2, "0883" : 2, "0884" : 2, "0885" : 2, "0887" : 2, "0889" : 2, "0892" : 2, "0893" : 2, "0894" : 2, "0895" : 2,
			"0896" : 2, "0897" : 2, "0898" : 2, "0920" : 2, "0930" : 2, "0940" : 2, "0942" : 2, "0943" : 2, "0944" : 2, "0946" : 2, "0947" : 2, "0948" : 2,
			"0949" : 2, "0950" : 2, "0952" : 2, "0954" : 2, "0955" : 2, "0956" : 2, "0957" : 2, "0959" : 2, "0964" : 2, "0965" : 2, "0966" : 2, "0967" : 2,
			"0968" : 2, "0969" : 2, "0972" : 2, "0973" : 2, "0974" : 2, "0977" : 2, "0978" : 2, "0979" : 2, "0980" : 2, "0982" : 2, "0983" : 2, "0984" : 2,
			"0985" : 2, "0986" : 2, "0987" : 2, "0993" : 2, "0994" : 2, "0995" : 2, "0996" : 2, "0997" : 2,
			"0180" : 3, "0570" : 3, "0800" : 3, "0990" : 3, "0120" : 3,
		},
		3 : {
			"011" : 3, "015" : 3, "017" : 3, "018" : 3, "019" : 3, "022" : 3, "023" : 3, "024" : 3, "025" : 3, "026" : 3, "027" : 3, "028" : 3, "029" : 3,
			"042" : 3, "043" : 3, "044" : 3, "045" : 3, "046" : 3, "047" : 3, "048" : 3, "049" : 3, "052" : 3, "053" : 3, "054" : 3, "055" : 3, "058" : 3,
			"059" : 3, "072" : 3, "073" : 3, "075" : 3, "076" : 3, "077" : 3, "078" : 3, "079" : 3, "082" : 3, "083" : 3, "084" : 3, "086" : 3, "087" : 3,
			"088" : 3, "089" : 3, "092" : 3, "093" : 3, "095" : 3, "096" : 3, "097" : 3, "098" : 3, "099" : 3,
			"050" : 4, "020" : $STRICT ? 3 : 4, "070" : $STRICT ? 3 : 4, "080" : $STRICT ? 3 : 4, "090" : $STRICT ? 3 : 4,
		},
		2 : {
			"03" : 4, "04" : 4, "06" : 4,
		}
	};
	// 市外局番の桁数を取得して降順に並べ替える
	var code = [];
	for(num in group){
		code.push(num * 1);
	}
	code.sort(function($a, $b){ return ($b - $a); });
	// 入力文字から数字以外を削除してnumber変数に格納する
	var number = String($INPUT).replace(/[０-９]/g, function($s){
	                  return String.fromCharCode($s.charCodeAt(0) - 65248);
	              }).replace(/\D/g, "");
	// 電話番号が10～11桁じゃなかったらfalseを返して終了する
	if(number.length < 10 || number.length > 11){
		return false;
	}
	// 市外局番がどのグループに属するか確認していく
	for(var i = 0, n = code.length; i < n; i++){
		var leng = code[i];
		var area = number.substr(0, leng);
		var city = group[leng][area];
		// 一致する市外局番を見付けたら整形して整形後の電話番号を返す
		if(city){
			return area + "-"
			         + number.substr(leng, city)
			           + (number.substr(leng + city) !== "" ?
			              "-" + number.substr(leng + city) : "");
		}
	}
};
window.addEventListener("DOMContentLoaded", function(){
	let elements = document.getElementsByClassName("TEL");
	let count = elements.length;
	// console.log("TELエレメント数：" + count);
	for (let i = 0; i < count; i++) {
		// console.log(elements[i].id);
		elements[i].addEventListener("change", function(){
			var p = getFormatPhone(this.value);
			if(p){
				this.value = p;
			}
		}, false);
	}
	// document.getElementById("telNo1").addEventListener("change", function(){
	// 	var p = getFormatPhone(this.value);
	// 	if(p){
	// 		this.value = p;
	// 	}
	// }, false);
}, false);
