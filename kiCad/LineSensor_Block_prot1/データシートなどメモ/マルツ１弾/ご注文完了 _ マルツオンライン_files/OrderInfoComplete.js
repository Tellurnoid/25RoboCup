var OrderInfoComplete = function() {
    this.initialize.apply(this, arguments);
}

OrderInfoComplete.prototype = {
    /** 注文リスト */ 
    orderList: null,

    checkMypageButtonAction : function() {
        var isError = false;
        if (isError) {
            existErrorAction();
        }
        return isError;
    },

    checkReturnButtonAction : function() {
        var isError = false;
        if (isError) {
            existErrorAction();
        }
        return isError;
    },

    checkTopGroupNameInput: function(form, required, indexSymbol) {
        if (form == null || form == undefined) {
            return false;
        }
        var isError = false;
        if (isNullorUndefined(indexSymbol)) {
            indexSymbol = "";
        }
        isError = this.checkOrderCompleteInput(form, required, indexSymbol) || isError;
        return isError;
    },

    checkOrderCompleteInput: function(form, required, indexSymbol) {
        if (form == null || form == undefined) {
            return false;
        }
        var isError = false;
        if (isNullorUndefined(indexSymbol)) {
            indexSymbol = "";
        }
        isError = this.checkInputCompleteAreaInput(form, required, indexSymbol) || isError;
        return isError;
    },

    checkInputCompleteAreaInput: function(form, required, indexSymbol) {
        if (form == null || form == undefined) {
            return false;
        }
        var isError = false;
        if (isNullorUndefined(indexSymbol)) {
            indexSymbol = "";
        }
        isError = this.checkOrderNoGroupInput(form, required, indexSymbol) || isError;
        isError = this.checkTotalPriceAreaInput(form, required, indexSymbol) || isError;
        return isError;
    },

    checkOrderNoGroupInput: function(form, required, indexSymbol) {
        if (form == null || form == undefined) {
            return false;
        }
        var isError = false;
        if (isNullorUndefined(indexSymbol)) {
            indexSymbol = "";
        }
        return isError;
    },

    checkTotalPriceAreaInput: function(form, required, indexSymbol) {
        if (form == null || form == undefined) {
            return false;
        }
        var isError = false;
        if (isNullorUndefined(indexSymbol)) {
            indexSymbol = "";
        }
        return isError;
    },

    checkSearchKeyInput: function(form, required) {
        if (form == null || form == undefined) {
            return false;
        }
        var isError = false;
        return isError;
    },

    onclickMypageButtonAction : function(sender, e) {
        submit("goMypage");
        return true;
    },

    onclickReturnButtonAction : function(sender, e) {
        submit("return");
        return true;
    },

    initialize: function(form) {
        var orderListName = "orderList";
        var orderListDepth = 0;
        this.orderList = initStandardList(form, orderListName, orderListDepth);


        var isErrorElementFocus = Validator.focusToFirstErrorElement();
        if (!isErrorElementFocus) {
            initFocus();
        }
    }

}

var screenItem;
function initOrderInfoComplete() {
    var form = document.forms["formMain"];
    screenItem = new OrderInfoComplete(form);
    initializeScreen(form, screenItem);
}

/**
 * 初期化処理です。追加の初期化処理を行う場合は、このメソッドをオーバーライドしてください。
 * @param form フォーム
 * @param screenItem 画面アイテム情報
 */
function initializeScreen(form, screenItem) {

}

function orderListSelectByIndex(index, indexSymbol, isFocusToListSequence) {
    if (isAlreadySelected(screenItem.orderList, index)) {
        return
    }
    var form = document.forms["formMain"];
    selectListByIndex(form, screenItem.orderList, index, indexSymbol);
}

function onkeydownOrderList(sender, e, indexSymbol) {
    if (isIgnoreOnkeydownEvent(e.target)) {
        // 明細のキーイベントを無視する
        return true;
    }
    var form = document.forms["formMain"];
    var selected = form.elements["orderListSelectedIndex" + indexSymbol];
    var selectedIndex = parseInt(selected.value);
    if (selectedIndex.toString() != "NaN") {
        if (e.keyCode == 13) {
            // リターンキー押下
            orderListSelectedRowReturned(selectedIndex);
            return false;
        } else if (e.keyCode == 38) {
            // 上
            if (selectedIndex - 1 >= 0) {
                var nextIndex = selectedIndex - 1;
                orderListSelectByIndex(nextIndex, indexSymbol + "_" + nextIndex, true);
            }
            return false;
        } else if (e.keyCode == 40) {
            // 下
            var nextIndex = selectedIndex + 1;
            orderListSelectByIndex(nextIndex, indexSymbol + "_" + nextIndex, true);
            return false;
        } else {
            // その他のキー
            return true;
        }
    } else {
        return true;
    }
}

function orderListSelectedRowReturned(index) {
}

function onmouseoverOrderListRow(sender, e, rowIndex, indexSymbol) {
    if (screenItem != null && screenItem != undefined) {
        if (screenItem.orderList != null && screenItem.orderList != undefined) {
            screenItem.orderList.setRowMoveColor(sender);
        }
    }
}

function onmouseoutOrderListRow(sender, e, rowIndex, indexSymbol) {
    if (screenItem != null && screenItem != undefined) {
        if (screenItem.orderList != null && screenItem.orderList != undefined) {
            screenItem.orderList.resetRowMoveColor(sender);
        }
    }
}

function onclickOrderListRow(sender, e, rowIndex, indexSymbol) {
    orderListSelectByIndex(rowIndex, indexSymbol, true);
}

function ondblclickOrderListRow(sender, e, rowIndex, indexSymbol) {
    orderListSelectByIndex(rowIndex, indexSymbol, true);
}

function sortOrderList(sortColumn, defaultSortType) {
    var form = document.forms["formMain"];
    var action = sortOrderListActionName();
    sort(sortColumn, defaultSortType, form, screenItem.orderList, action);
}

function sortOrderListActionName() {
    return null;
}

function onchangeSortSelectOrderList(sortSelector) {
    sortValue = sortSelector.options[sortSelector.selectedIndex].value;
    if (sortValue != "") {
        var form = document.forms["formMain"];
        var action = onchangeSortSelectOrderListActionName();
        sortColumn = sortSelector.options[sortSelector.selectedIndex].value.split("_")[0];
        sortType = sortSelector.options[sortSelector.selectedIndex].value.split('_')[1];
        sort(sortColumn, sortType, form, screenItem.orderList, action);
    }
}

function onchangeSortSelectOrderListActionName() {
    return null;
}

