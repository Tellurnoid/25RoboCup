/**
 * ご注文情報完了 のJavaScript実装ファイルです。
 * standard.js及び、eltex.standard.jsのfunctionを利用しています。
 * 履歴   : WebSystemGenerator-2.0.0, Created
 * @since 2013/07/18
 */
 
var screenItem;
$(document).ready(function() {
    var form = document.forms["formMain"];
    screenItem = new OrderInfoComplete(form);
});

/**
 * 戻る(F8)に対し、クリックされた際に実行されるイベント処理です。
 * @param sender 対象オブジェクト(戻る(F8))
 * @param e イベント
 */
function onclickReturnButton(sender, e) {
    if (isNullorUndefined(screenItem)) {
        return false;
    }

    var isError = screenItem.checkReturnButtonAction();
    if (isError) {
        return false;
    }

    screenItem.onclickReturnButtonAction(sender, e);
    return true;
}

/**
 * マイページへに対し、クリックされた際に実行されるイベント処理です。
 * @param sender 対象オブジェクト(マイページへ)
 * @param e イベント
 */
function onclickMypageButton(sender, e) {
    if (isNullorUndefined(screenItem)) {
        return false;
    }

    var isError = screenItem.checkMypageButtonAction();
    if (isError) {
        return false;
    }

    screenItem.onclickMypageButtonAction(sender, e);
    return true;
}

/**
 * ログアウトに対し、クリックされた際に実行されるイベント処理です。
 * @param sender 対象オブジェクト(ログアウト)
 * @param e イベント
 */
function onclickLogoutButton(sender, e) {
    if (isNullorUndefined(screenItem)) {
        return false;
    }

    var isError = screenItem.checkLogoutButtonAction();
    if (isError) {
        return false;
    }

    screenItem.onclickLogoutButtonAction(sender, e);
    return true;
}
