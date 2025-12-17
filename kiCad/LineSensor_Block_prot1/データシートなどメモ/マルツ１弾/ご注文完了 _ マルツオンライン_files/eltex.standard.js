/**
 * eltex.standard: WebSystemGenerator用JavaScript定義ファイルです。
 * standard.jsのfunctionを利用しています。
 * 著作権 : Copyright (c) 2011 ELTEX, Inc. All Rights Reserved.<br />
 * 会社名 : ELTEX, Inc.<br />
 * 履歴   : 2011/02/25, 1.0.0, Jiro Saito, Created
 * @since 2011/02/25
 * @version 1.0.0
 */

/**
 * リストを初期化し、返却します。
 * @param form フォーム
 * @param tableId リストID
 * @returns 初期化されたリスト
 */
function initStandardList(form, tableId, depth) {
    if (form == null || form == undefined) {
        return null;
    }

    var tableElements = new Array();
    var table = document.getElementById(tableId);
    if (table != null && table != undefined) {
    	tableElements[0] = table;
    } else {
        tableElements[0] = new Array();
    	tableElements = getTableElements(tableElements, tableId, 0, depth);
    }

    // 明細in明細対応
    if (tableElements != null && tableElements[0] != null && tableElements[0] != undefined) {
    	var list = new StandardList(tableId, tableElements);
    	var selectedIndexId = tableId + "SelectedIndex";
    	var element = form.elements[selectedIndexId];
    	if (element != null && element != undefined) {
    		var selectedIndex = parseInt(element.value);
    		selectListByIndex(form, list, selectedIndex);
    	} else {
    		setSelectedIndex(list, tableElements, "", 0, depth);
    	}

    	// sortIcon
    	setSortIcon(tableId);

        return list;
    }
}

function setSortIcon(tableId) {
	var sortTypeList = makeSortTypeList(tableId);
	for (var i = 0; i < sortTypeList.length; i++) {
		var sortType = sortTypeList[i];
		var sortLinkThId = tableId + capitalize(sortType.columnName) + "TitleLink";
		var sortLinkThElement = document.getElementById(sortLinkThId);
		if (isNullorUndefined(sortLinkThElement)) {
			continue;
		}

		if (sortType.sortType == "DESC") {
			sortLinkThElement.className = "sortIconDesc sortOrderDesc" + i;
		} else {
			sortLinkThElement.className = "sortIconAsc sortOrderAsc" + i;
		}
	}
}

/**
 * 対象テーブルIDのソートカラムのhidden領域を取得し、ソート順にリストでソート状態を取得します。
 * ソート状態は、
 *  columnName: カラム名
 *  sortType: ASC or DESC
 * で取得できます。
 * @param tableId 対象テーブルID
 * @returns ソート状態のリスト
 */
function makeSortTypeList(tableId) {
	var sortTypeList = new Array();
   	var sortColumnId = tableId + "SortColumn";
   	var sortColumnElement = document.getElementById(sortColumnId);
   	if (sortColumnElement == null || sortColumnElement == undefined) {
   		return sortTypeList;
   	}
   	var sortColumnValue = sortColumnElement.value;
	if (!isNullorUndefinedorEmpty(sortColumnValue)) {
		var sortColumnAndTypes = sortColumnValue.split(",");
		for (var i = 0; i < sortColumnAndTypes.length; i++) {
			var sortColumnAndType = sortColumnAndTypes[i];
			if (isNullorUndefinedorEmpty(sortColumnAndType)) {
				continue;
			}

			// trim
			sortColumnAndType = sortColumnAndType.replace(/^[\s]+|[\s]+$/g, "");

			var separateSortColumnAndType = sortColumnAndType.split(":");
			var sortColumnString = separateSortColumnAndType[0];
			var sortTypeValue = "ASC";
			if (separateSortColumnAndType.length > 1) {
				sortTypeValue = separateSortColumnAndType[1];
			}

			var sortColumnType = new Object();
			sortColumnType.columnName = sortColumnString;
			sortColumnType.sortType = sortTypeValue;
			sortTypeList.push(sortColumnType);
		}
	}
	return sortTypeList;
}

/**
 * 該当IDのテーブルエレメントを再帰的に全て取得します。
 * @param elements テーブルエレメントの多次元配列
 * @param tableId 基本テーブルID
 * @param index 探索中インデックス
 * @param depth 残りの深さ
 * @returns 渡されたエレメント以下の全てのテーブルエレメントの多次元配列
 */
function getTableElements(elements, tableId, index, depth) {
	if (depth > 0) {
		elements[index] = getTableElements(elements[index], tableId + "_" + index, 0, depth - 1);
		if (elements[index] != null && elements[index] != undefined) {
			if (depth > 1) {
				elements[index + 1] = new Array();
			}
			elements = getTableElements(elements, tableId, index + 1, depth);
			if (elements == null) {
				return null;
			}
		} else {
			elements.length = index;
		}
		return elements.length > 0 ? elements : null;
	} else {
		return document.getElementById(tableId);
	}
}

/**
 * 該当テーブルの選択行を再帰的に全て設定します。
 * @param list StandardList
 * @param elements テーブルエレメント配列
 * @param symbol インデックスシンボル
 * @param index 現在インデックス
 * @param depth 現在深さ
 */
function setSelectedIndex(list, elements, symbol, index, depth) {
	if (depth > 0) {
		var array = elements[index];
		if (!isNullorUndefined(array)) {
			var newSymbol = symbol + "_" + index;
			setSelectedIndex(list, array, newSymbol, 0, depth - 1);
			setSelectedIndex(list, elements, symbol, index + 1, depth);
		}
	} else {
		var indexElement = document.getElementById(list.tableId + "SelectedIndex" + symbol);
		if (!isNullorUndefined(indexElement) && !isNullorUndefinedorEmpty(indexElement.value)) {
			var rowId = list.tableId + "Row" + symbol + "_" + indexElement.value;
			if (!isNullorUndefined(elements.rows)) {
				list.selectRow(elements.rows.namedItem(rowId));
			}
		}
	}
}

/**
 * Getパラメータに従って、フォーカスを設定します。
 */
function initFocus() {
	var focusParameterName = "focusItemId";
	var focusIdItem = document.getElementById(focusParameterName);
	if (isNullorUndefined(focusIdItem) || isNullorUndefinedorEmpty(focusIdItem.value)) {
		return;
	}
	focusToItem(focusIdItem.value);
}

/**
 * 指定されたID,Nameを持った画面状態を保存します。
 * オブジェクトの以下のプロパティに状態が保存されます。
 *  texts: テキストの値
 *  checks: チェックの状態(ラジオボタン、チェックボックスのチェック状態が配列で保存)
 *  selects: 選択の状態(コンボボックスの選択インデックスの値)
 * @param form フォーム
 * @param textIds
 * @param checkBoxNames
 * @param comboBoxIds
 * @param radioNames
 * @returns 保存されたオブジェクト
 */
function saveScreenObject(form, textIds, checkBoxNames, comboBoxIds, radioNames) {
    if (form == null || form == undefined) {
        return null;
    }

    var saveObject = new Object();

    var texts = new Object();
    var checks = new Object();
    var selects = new Object();

    saveObject.texts = texts;
    saveObject.checks = checks;
    saveObject.selects = selects;

    // text
    if (textIds != null && textIds != undefined) {
        for (var i = 0; i < textIds.length; i++) {
            var textId = textIds[i];
            var element = document.getElementById(textId);
            if (element != null && element != undefined) {
                texts[textId] = element.value;
            }
        }
    }

    // radio
    if (radioNames != null && radioNames != undefined) {
        for (var i = 0; i < radioNames.length; i++) {
            var radioName = radioNames[i];
            var element = form[radioName];
            if (element != null && element != undefined) {
                var radioChecks = new Array();
                for (var j = 0, e; e = form[radioName][j]; j++) {
                    radioChecks[j] = e.checked;
                }
                checks[radioName] = radioChecks;
            }
        }
    }

    // checkbox
    if (checkBoxNames != null && checkBoxNames != undefined) {
        for (var i = 0; i < checkBoxNames.length; i++) {
            var checkBoxName = checkBoxNames[i];
            var element = form[checkBoxName];
            if (element != null && element != undefined) {
                var checkBoxChecks = new Array();
                for (var j = 0, e; e = form[checkBoxName][j]; j++) {
                    checkBoxChecks[j] = e.checked;
                }
                checks[checkBoxName] = checkBoxChecks;
            }
        }
    }

    // combobox
    if (comboBoxIds != null && comboBoxIds != undefined) {
        for (var i = 0; i < comboBoxIds.length; i++) {
            var comboBoxId = comboBoxIds[i];
            var element = document.getElementById(comboBoxId);
            if (element != null && element != undefined) {
            	if (!isNullorUndefined(element.multiple) && !element.multiple) {
	                selects[comboBoxId] = element.selectedIndex;
	            } else {
	                var multipleSelects = new Array();
	                for (var j = 0; j < element.options.length; j++) {
	                    multipleSelects[j] = element.options[j].selected;
	                }
	                selects[comboBoxId] = multipleSelects;
	            }
            }
        }
    }

    return saveObject;
}

/**
 * 指定された画面状態に戻します。
 * screenObjectには、以下のプロパティで保存されているものとします。
 * saveScreenObjectで保存されたオブジェクトです。
 *  texts: テキストの値
 *  checks: チェックの状態(ラジオボタン、チェックボックスのチェック状態が配列で保存)
 *  selects: 選択の状態(コンボボックスの選択インデックスの値)
 * @param form フォーム
 * @param screenObject 画面状態オブジェクト
 */
function resetScreenObject(form, screenObject) {
    if (isNullorUndefined(form)) {
        return;
    }

    if (isNullorUndefined(saveScreenObject)) {
        return;
    }

    var texts = screenObject.texts;
    if (!isNullorUndefined(texts)) {
        for (var textId in texts) {
            var element = document.getElementById(textId);
            if (!isNullorUndefined(element)) {
                element.value = texts[textId];
            }
        }
    }

    var selects = screenObject.selects;
    if (!isNullorUndefined(selects)) {
        for (var selectId in selects) {
            var element = document.getElementById(selectId);
           	if (!isNullorUndefined(element.multiple) && !element.multiple) {
	            if (!isNullorUndefined(element)) {
	                element.selectedIndex = selects[selectId];
	            }
	        } else {
                var selectArray = selects[selectId];
                if (!isNullorUndefined(selectArray)) {
                	for (var i = 0; i < element.options.length; i++) {
                		if (selectArray.length > i) {
		                	element.options[i].selected = selectArray[i];
		                }
                	}
                }
	        }
        }
    }

    var checks = screenObject.checks;
    if (!isNullorUndefined(checks)) {
        for (var checkName in checks) {
            var elements = form[checkName];
            if (!isNullorUndefined(element)) {
                var checkArray = checks[checkName];
	            if (!isNullorUndefined(checkArray)) {
                    for (var i = 0, e; e = elements[i]; i++) {
                		if (checkArray.length > i) {
	                        e.checked = checkArray[i];
	                    }
                    }
                }
            }
        }
    }
}

/**
 * 指定されたリストの行をインデックスを指定して選択します。
 * @param form フォーム
 * @param list StandardListオブジェクト
 * @param index 行インデックス
 * @param indexSymbol インデックスシンボル
 */
function selectListByIndex(form, list, index, indexSymbol) {
    if (form == null || form == undefined) {
        return;
    }

    if (list == null || list == undefined) {
        return;
    }

    if (index != null && index != undefined) {
        var selectedIndex = parseInt(index);
        var selectedRow = null;
        if (selectedIndex.toString() != "NaN" || indexSymbol != undefined) {
            var listId = list.tableId;
        	if (indexSymbol == undefined) {
	            selectedRow = listId + "Row_" + selectedIndex;
        	} else {
        		selectedRow = listId + "Row" + indexSymbol;
        	}
            var rowElement = document.getElementById(selectedRow);
            if (rowElement != null && rowElement != undefined) {
                list.selectRowById(selectedRow);
                var selectedIndexId;
                if (indexSymbol != null && indexSymbol != undefined) {
                	var symbolList = indexSymbol.split("_");
                	if (symbolList.length > 2) {
	                	selectedIndexId = listId + "SelectedIndex" + indexSymbol;
	                } else {
	                	selectedIndexId = listId + "SelectedIndex";
	                }
            	} else {
                	selectedIndexId = listId + "SelectedIndex";
            	}
                var selected = form.elements[selectedIndexId];
                if (selected != null && selected != undefined) {
                    selected.value = index.toString();
                }
            }
        }
    }
}

/**
 * リストの指定されたページへ移動します。
 * @param form フォーム
 * @param list StandardListオブジェクト
 * @param action submitで送信されるアクションの種類
 * @param page ページ番号
 */
function changePage(form, list, action, page) {
    if (form == null || form == undefined) {
        return;
    }

    if (list == null || list == undefined) {
        return;
    }

    var currentPageName = list.tableId + "ShowPageNumber";
    var currentPage = form.elements[currentPageName]
    if (currentPage == null) {
        return false;
    }
    var pageNumber;
    Validator.init(currentPage);
    try {
        pageNumber = new ExNumber(page);
    } catch (e) {
        // 数字以外の文字が入力された場合
        Validator.output(currentPage, e.message);
        return false;
    }

    if (page < 1) {
        page = 1
    }

    // Formへの入力値を元に戻す(最後に検索された条件でページング)
    var preCriteria = list.criteria;
    if (preCriteria != null && preCriteria != undefined) {
        resetScreenObject(form, preCriteria);
    }

    var actionTypeValue = "paging";
    if (!isNullorUndefined(action)) {
    	actionTypeValue = action;
    }

    var actionType = form["a"];
    if (!isNullorUndefined(actionType)) {
        actionType.value = actionTypeValue;
    }

    currentPage.value = pageNumber.toString();
    disableCardInfo();
    form.submit();
}

/**
 * サブミット処理です。
 * @param action アクションの種類
 * @param sender 送信オブジェクト
 * @param timeout タイムアウト時間(msec)
 * @param waitDialogValidTime submitからWaitDialog表示までの時間(msec)
 */
function submit(action, sender, timeout, waitDialogValidTime) {
    var form = document.forms["formMain"];
    if (form != null && form != undefined) {
        var actionType = form.elements["a"];
        if (actionType != null && actionType != undefined) {
            if (!isNullorUndefined(action)) {
	            actionType.value = action;
            }

            if (isNullorUndefined(timeout)) {
            	// 設定なしの場合3秒
            	timeout = 3000;
            }

            // 多重リクエスト防止
            var changedTargetIdList = changeDisableButton(form, true, null);
            setTimeout(function() {
            	changeDisableButton(form, false, changedTargetIdList)
            }, timeout);

            disableCardInfo();

            form.submit();

            if (isNullorUndefined(waitDialogValidTime)) {
            	// 設定なければ、0.5秒
            	waitDialogValidTime = 500;
            }

            if (waitDialogValidTime >= 0) {
	            setTimeout(function() {
	            	// waitDialogValidTime(ms)以上かかる処理があればWaitDialogを表示
					showWaitDialog();
	            }, waitDialogValidTime);
	        }
        }
    }
}

/**
 * ボタンの有効/無効を切り替えます。
 * @param form フォーム
 * @param isDisable 無効にするか
 * @param targetIdList ボタンを有効/無効にする対象エレメントIDのリスト(対象がなければ画面全てのボタンを対象にする)
 * @return 状態を変更されたエレメントIDのリスト
 */
function changeDisableButton(form, isDisable, targetIdList) {
	if (isNullorUndefined(targetIdList) || targetIdList.length == 0) {
		// フォームの全てのボタン対象
		targetIdList = new Array();
	    for (var i = 0; i < form.length; i++) {
			var element = form.elements[i];
			if (element.type == 'button' || element.type == 'submit') {
				targetIdList.push(element.id);
			}
	    }
	}

	var changedTargetIdList = new Array();
    for (var i = 0; i < targetIdList.length; i++) {
		var element = document.getElementById(targetIdList[i]);
		if (isNullorUndefined(element)) {
			continue;
		}
		if (element.type == 'button' || element.type == 'submit') {
            if (element.disabled != undefined) {
            	if (element.disabled != isDisable) {
	                element.disabled = isDisable;
	                changedTargetIdList.push(element.id);
	            }
            }
        }
    }
    return changedTargetIdList;
}

/**
 * アップロード処理を実行します。
 * @param action アップロードAction
 * @param url アップロードURL(省略した場合、現在のform actionのjspをuploadに置換した値でsubmit)
 */
function uploadFile(action, url) {
    var form = document.forms["formMain"];
    var multipartConstants = "multipart/form-data";
    form.encoding = multipartConstants;
    form.enctype = multipartConstants;

    if (url != null && url != undefined) {
        form.action = url;
    } else {
        var uploadUrl = form.action.replace("\.jsp", "\.upload");
        form.action = uploadUrl;
    }
    submit(action);
}

/**
 * 指定された一覧が選択されているか確認します。
 * @return 一覧が選択されている場合true
 */
function isListSelected(list) {
    if (list == null || list == undefined) {
        return false;
    }
    var form = document.forms["formMain"];
    var selectedIndexName = list.tableId + "SelectedIndex";
    var selected = form.elements[selectedIndexName];
    var selectedIndex = parseInt(selected.value);
    if (selectedIndex.toString() != "NaN") {
        return true;
    } else {
        return false;
    }
}

/**
 * 選択されたリストから指定した項目の値を取得します。
 * @param list リスト
 * @param id 項目のID
 * @returns 項目の値
 */
function getListSelectedValue(list, id) {
    var innerObj = getListSelectedObject(list, id, "_innerId");
    if (innerObj != null && innerObj != undefined) {
    	return getValueFromElement(innerObj);
	} else {
	    var obj = getListSelectedObject(list, id);
    	return getValueFromElement(obj);
	}
}

/**
 * 選択されたリストから指定した項目を取得します。
 * @param list リスト
 * @param id 項目のID
 * @param suffix IDのサフィックス
 * @returns 項目の値
 */
function getListSelectedObject(list, id, suffix) {
    var index = getListSelectedIndex(list);
    if (isNullorUndefinedorEmpty(index)) {
    	return null;
    }
    var idValue = id + "_" + index;
    if (suffix != null && suffix != undefined) {
    	idValue = idValue + suffix;
    }
    var idElement = document.getElementById(idValue);
    if (idElement != null && idElement != undefined) {
	    return idElement;
	} else {
		return null;
	}
}

/**
 * 指定されたリストの選択行のインデックスを取得します。
 * @param list リスト
 * @returns 選択行のインデックス
 */
function getListSelectedIndex(list) {
	if (isNullorUndefined(list)) {
		return null;
	}
    var form = document.forms["formMain"];
    var selectedIndexName = list.tableId + "SelectedIndex";
    var selected = form.elements[selectedIndexName];
    if (isNullorUndefined(selected)) {
    	return null;
    }

    return selected.value;
}

/**
 * 指定されたリストの指定インデックスの行が既に選択されているか返却します。
 * @param list リスト
 * @param index 選択するインデックス
 * @returns 指定されたインデックスの行が既に選択されている場合true
 */
function isAlreadySelected(list, index) {
    var nowIndex = getListSelectedIndex(list);
   	if (isNullorUndefinedorEmpty(nowIndex) || nowIndex != index) {
   		return false;
   	} else {
	   	return true;
   	}
}

/**
 * 選択されたリストから指定した項目を取得します。
 * @param list リスト
 * @param id 項目のID
 * @returns 項目の値
 */
function getListSelectedLabel(list, id) {
    return getListSelectedObject(list, id, "_innerId");
}

/**
 * 指定されたリストから行とIDを指定し項目を取り出します。
 * @param list リスト
 * @param id 項目のID
 * @param index 行インデックス
 */
function getItemFromList(list, id, index) {
    var form = document.forms["formMain"];
    return form.elements[id + "_" + index.toString()];
}

/**
 * 指定されたリストから行とIDを指定し値を取り出します。
 * @param list リスト
 * @param id 項目のID
 * @param index 行インデックス
 */
function getItemValueFromList(list, id, index) {
	var listId = id + "_" + index.toString();
	return getElementValue(listId);
}

/**
 * ソート処理を実行します。
 * ソート処理を実施するために、Hidden領域として以下の2つが必要です。
 *  現在のソート実施カラム名領域: (一覧ID) + SortColumn
 *  現在のソート実施タイプ(ASC or DESC)領域: (一覧ID) + SortType
 * 現在のソート実施カラム名と引数指定のソートカラム名が同一の場合、ソートを反転させます。
 * 同一でない場合は、デフォルトのソート実施タイプでソートを実施します。
 *  ただし、デフォルトのソート実施タイプが設定されていない場合はASCでソートされます。
 * @param sortColumn ソートカラム
 * @param defaultSortType デフォルトのソート実施タイプ(ASC or DESC)
 * @param form フォーム
 * @param list StandardListオブジェクト
 * @param action submitアクション(nullの場合はformのアクションをsubmit)
 */
function sort(sortColumn, defaultSortType, form, list, action) {
    var sortType = judgeSortTypeFromNowSettingAndDefaultSortType(list.tableId, sortColumn, defaultSortType);

    var settingSortColumnName = list.tableId + "SortColumn";
    var settingSortColumn = form.elements[settingSortColumnName]
    if (isNullorUndefined(settingSortColumn)) {
        return;
    }

    settingSortColumn.value = sortColumn + ":" + sortType;
    changePage(form, list, action, 1);
}

/**
 * ソート状態をデフォルトの状態にし、検索します。
 * @param form フォーム
 * @param list StandardListオブジェクト
 * @param action submitアクション(nullの場合はformのアクションをsubmit)
 */
function defaultSort(form, list, action) {
    clearSortColumn(list.tableId, form);
    changePage(form, list, action, 1);
}

/**
 * ソート状態をデフォルトの状態にします。
 * @param tableId tableId
 * @param form フォーム(未設定の場合、formMainを設定)
 */
function clearSortColumn(tableId, form) {
	if (isNullorUndefined(form)) {
    	form = document.forms["formMain"];
	}
    var settingSortColumnName = tableId + "SortColumn";

    var settingSortColumn = form.elements[settingSortColumnName]
    if (isNullorUndefined(settingSortColumn)) {
        return;
    }
    settingSortColumn.value = "";
}

/**
 * 現状の設定と指定されたソートによって、ソートタイプ(ASC or DESC)を判定します。
 * @param tableId テーブルID
 * @param sortColumn 指定されたソートカラム名
 * @param defaultSortType 指定されたソートカラムの初期ソートタイプ(ASC or DESC)
 * @returns ソートタイプ(ASC or DESC)
 */
function judgeSortTypeFromNowSettingAndDefaultSortType(tableId, sortColumn, defaultSortType) {
    var sortTypeList = makeSortTypeList(tableId);
    var asc = "ASC";
    var desc = "DESC";

    if (sortTypeList.length != 1) {
    	// 画面からは複数ソートカラムを指定できないので、一つのソートカラム指定以外の場合、デフォルトの一つのカラムでソートさせる
    	return defaultSortType.toUpperCase();
    } else {
    	var settingSortColumnValue = sortTypeList[0].columnName;
    	var settingSortTypeValue = sortTypeList[0].sortType;
	    if (settingSortColumnValue == sortColumn) {
	        if (settingSortTypeValue.toUpperCase() == asc) {
	            sortType = desc;
	        } else {
	            sortType = asc;
	        }
	    } else {
	        if (!isNullorUndefined(defaultSortType) && (defaultSortType.toUpperCase() == asc || defaultSortType.toUpperCase() == desc)) {
	            sortType = defaultSortType;
	        } else {
	            sortType = asc;
	        }
	    }
	    return sortType;
	}
}

/**
 * 指定されたラベルに対し、必須ラベルの設定を行います。
 * @param labelName ラベルID
 */
function changeRequiredLabelColor(labelName, isSetting) {
	changeClassName(labelName, "REQUIRED_LABEL", isSetting)
}

/**
 * 指定された要素に対し、disableを解除し、hidden要素が存在すれば削除します。
 * ラジオボタンは対象外です。
 * @param id 対象要素のid
 */
function changeEnabled(id) {
	if(id == null || id == undefined) {
		return;
	}
	var elementDisabledHidden = document.getElementById(id + '_disabled_hidden');
	var element = document.getElementById(id);
	if(element != null && element != undefined) {
		if(element.type.toUpperCase() == 'TEXT') {
			element.readOnly = false;
		} else {
			element.disabled = false;
		}
		element.className = element.className.replace(/\s?readonly/g, "");

		if(element.type.toUpperCase() != 'TEXT') {
			if (elementDisabledHidden != null && elementDisabledHidden != undefined) {
				elementDisabledHidden.parentNode.removeChild(elementDisabledHidden);
			}
		}
	}
}

/**
 * 指定されたラジオボタンの要素に対し、disableを解除し、hidden要素が存在すれば削除します。
 * @param name ラジオボタンのname
 */
function changeEnabledRadio(name) {
	if(name == null || name == undefined) {
		return;
	}
	var elementDisabledHidden = document.getElementById(name + '_disabled_hidden');
	var elements = document.getElementsByName(name);
	if (elements != null && elements != undefined) {
		for(var i = 0; i < elements.length; i++) {
			elements[i].disabled = false;
			elements[i].className = elements[i].className.replace(/\s?readonly/g, "");
		}
		if (elementDisabledHidden != null && elementDisabledHidden != undefined) {
			elementDisabledHidden.parentNode.removeChild(elementDisabledHidden);
		}
	}
}

/**
 * 指定された要素に対し、disableを設定し、hidden要素が存在しなければ追加します。
 * ラジオボタンは対象外です。
 * @param targetElement 対象要素のid
 */
function changeDisabled(id) {
	if(id == null || id == undefined) {
		return;
	}
	var elementDisabledHidden = document.getElementById(id + '_disabled_hidden');
	var element = document.getElementById(id);
	if(element != null && element != undefined) {
		if(element.type.toUpperCase() == 'TEXT') {
			element.readOnly = true;
		} else {
			element.disabled = true;
		}
		element.className = element.className.indexOf(" readonly") > 0 ? element.className: element.className + " readonly";
		if(element.type.toUpperCase() != 'TEXT') {
			if (elementDisabledHidden == null || elementDisabledHidden == undefined) {
				elementDisabledHidden = document.createElement('input');
				elementDisabledHidden.type = 'hidden';
				elementDisabledHidden.id = id + '_disabled_hidden';
				elementDisabledHidden.name = element.name;

				if(element.type.toUpperCase() == 'CHECKBOX') {
					if(element.checked) {
						elementDisabledHidden.value = element.value;
					}
				}
				if(element.tagName.toUpperCase() == 'SELECT') {
					var elementValue = "";
					if (element.selectedIndex != -1) {
						elementValue = element.options[element.selectedIndex].value;
					}
					elementDisabledHidden.value = elementValue;
				}
				element.parentNode.insertBefore(elementDisabledHidden, element);
			}
		}
	}
}

/**
 * 指定されたラジオボタン要素に対し、disableを設定し、hidden要素が存在しなければ追加します。
 * @param name ラジオボタンのname
 */
function changeDisabledRadio(name) {
	if(name == null || name == undefined) {
		return;
	}
	var elementDisabledHidden = document.getElementById(name + '_disabled_hidden');
	var elements = document.getElementsByName(name);

	if (elements != null && elements != undefined) {
		for(var i = 0; i < elements.length; i++) {
			if(elements[i].type != 'hidden') {
				elements[i].disabled = true;
				elements[i].className = elements[i].className.indexOf(" readonly") > 0 ? elements[i].className: elements[i].className + " readonly";
			}
		}
		if (elementDisabledHidden == null || elementDisabledHidden == undefined) {
			elementDisabledHidden = document.createElement('input');
			elementDisabledHidden.type = 'hidden';
			elementDisabledHidden.id = name + '_disabled_hidden';
			elementDisabledHidden.name = elements[0].name;
			if(elements[0].type.toUpperCase() == 'RADIO') {
				for(var i = 0; i < elements.length; i++) {
					if(elements[i].checked) {
						elementDisabledHidden.value = elements[i].value;
						break;
					}
				}
			}
		}
		elements[0].parentNode.insertBefore(elementDisabledHidden, elements[0]);
	}
}

/**
 * 指定されたURLのサブウィンドウダイアログ(参照選択画面)を表示します。
 * @param url URL(encodeされます)
 * @param width ダイアログ幅(px)
 * @param height ダイアログ高さ(px)
 * @param parameter 子に渡すGetパラメータ
 * @param setting  [親ウィンドウID]:[子ウィンドウID](複数の場合は、カンマ区切り)
 * @param focus 参照選択後、親ウィンドウに戻すフォーカスのID
 * @param isModeless trueの場合はモードレスでウィンドウを開きます
 */
function openRefSelectWindow(url, width, height, parameter, setting, focus, isModeless) {
	var p = "?";
	if (parameter != null && parameter != undefined && parameter != "") {
		p = p + parameter + "&"
	}

	var settingParameter = "settingParameter=" + setting;
	p = p + settingParameter;

	if (focus != null && focus != undefined && focus != "") {
		var fucusParameter = "&focusParameter=" + focus;
		p = p + fucusParameter;
	}

	url = url + p;

	if (!isNullorUndefined(isModeless) && isModeless) {
		return openModelessWindow(url, width, height);
	} else {
		return openWindow(url, width, height);
	}
}

/**
 * ポップアップのコールバック処理をセットします。
 * @param win (必須)モードレスポップアップウィンドウ
 * @param callback (必須)コールバックファンクション
 * @param timeout 待ち時間(ミリ秒)
 */
function setPopupCallback(win, callback, timeout) {
	if (isNullorUndefined(timeout)) {
		timeout = 500;
	}
    if (isNullorUndefined(win) || win.closed) {
    	callback.apply();
    } else {
    	setTimeout(function(){
    		setPopupCallback(win, callback);
    		}, timeout);
    }
}

/**
 * 親ウィンドウから以下のルールに基づいたパラメータを取得した場合、
 * パラメータルールに従って、子ウィンドウのリスト選択値を親ウィンドウに設定します。
 * 参照画面から利用します。
 * ルール: setting [親ウィンドウID]:[子ウィンドウID]:[上書きするか(t/f)]:[データ型](複数の場合は、カンマ区切り)
 * データ型は、親ウインドウの設定値が複数カラムに分かれている項目のみ使用
 * z:郵便番号2カラム t:電話番号3カラム
 * @param setting 親ウィンドウからのパラメータ設定値
 * @param list 子ウィンドウのリスト
 */
function setSelectedValueToParentWindowItems(setting, list) {
	if (setting == null || setting == undefined) {
		return;
	}

	var sp = setting.replace(/ /g, "");
	var settings = sp.split(",");
	for (var i = 0; i < settings.length; i++) {
		var setting = settings[i];
		var parentChildId = setting.split(":");
		if (parentChildId.length < 2) {
			continue;
		}

		var parentId = parentChildId[0];
		var childId = parentChildId[1];

		var overwrite = true;
		if (parentChildId.length > 2) {
			if (parentChildId[2] == "f") {
				overwrite = false;
			}
		}

		var parentDataType = "";
		if (parentChildId.length > 3 && parentChildId[3] != "") {
			parentDataType = parentChildId[3];
		}

		var targetValue;
		if (list == null || list == undefined) {
			targetValue = getElementValue(childId);
		} else {
			targetValue = getListSelectedValue(list, childId);
		}
		if (targetValue == null || targetValue == undefined) {
			continue;
		}

		if(parentDataType == "z"){
			var zip2col = targetValue.split("-");
			setValueToParentWindowItem(parentId + "Pre", zip2col[0], overwrite);
			setValueToParentWindowItem(parentId + "Pre" + "_innerId", zip2col[0], overwrite);
			if(zip2col.length>1){
				setValueToParentWindowItem(parentId + "Post", zip2col[1], overwrite);
				setValueToParentWindowItem(parentId + "Post" + "_innerId", zip2col[1], overwrite);
			}else{
				setValueToParentWindowItem(parentId + "Post", "", overwrite);
				setValueToParentWindowItem(parentId + "Post" + "_innerId", "", overwrite);
			}
		}else if(parentDataType == "t"){
			var tel3col = targetValue.split("-");
			setValueToParentWindowItem(parentId + "Pre", tel3col[0], overwrite);
			setValueToParentWindowItem(parentId + "Pre" + "_innerId", tel3col[0], overwrite);
			if(tel3col.length>1){
				setValueToParentWindowItem(parentId + "Middle", tel3col[1], overwrite);
				setValueToParentWindowItem(parentId + "Middle" + "_innerId", tel3col[1], overwrite);
			}else{
				setValueToParentWindowItem(parentId + "Middle", "", overwrite);
				setValueToParentWindowItem(parentId + "Middle" + "_innerId", "", overwrite);
			}
			if(tel3col.length>2){
				setValueToParentWindowItem(parentId + "Post", tel3col[2], overwrite);
				setValueToParentWindowItem(parentId + "Post" + "_innerId", tel3col[2], overwrite);
			}else{
				setValueToParentWindowItem(parentId + "Post", "", overwrite);
				setValueToParentWindowItem(parentId + "Post" + "_innerId", "", overwrite);
			}
		}else{
			setValueToParentWindowItem(parentId, targetValue, overwrite);
			setValueToParentWindowItem(parentId + "_innerId", targetValue, overwrite);
		}

	}
}

/**
 * 親ウィンドウに選択行の値とフォーカスを設定し、指定があれば画面を閉じます。
 * hidden値として、parameterSetting/parameterFocusが存在することを前提にしたメソッドです。
 * @param list リスト
 * @param isClose 設定後画面を閉じるか。
 */
function setSelectedToParentWindowItems(list, isClose) {
    var settingParameter = document.getElementById("settingParameter");
    if (settingParameter != null) {
	    setSelectedValueToParentWindowItems(settingParameter.value, list);
    }

	var focus = document.getElementById("focusParameter");
	if (focus != null && focus != undefined) {
		var focusId = focus.value;
		setFocusToParentWindowItem(focusId);
	}

	if (isClose) {
		// ウィンドウを閉じる
	    if(window.parent) {
			window.parent.close();
		}
		else {
			this.close();
		}
	}
}

/**
 * objの値を指定した日付フォーマットに変換します。
 *
 * @param obj オブジェクト(document.getElementByIdで取得)
 * @param formatType 日付フォーマットのタイプ(YYYYMMDD/YYMM/YYYYMMDDhhmiss/hhmm)
 */
function setDateValue(obj, formatType) {
	if (formatType == 'YYYYMMDD') {
		setFormatDate(target);
	} else if (formatType == 'YYMM') {
		setFormatDateYYMM(target);
	} else if (formatType == 'YYYYMMDDhhmiss') {
		setFormatDateTime(target);
	} else if (formatType == 'hhmm') {
		setFormatTime(target);
	} else {
		return;
	}
}

/**
 * 日付フォーマットされたvalueを取得します。
 *
 * @param value フォーマットする値
 * @param formatType 日付フォーマットのタイプ(YYYYMMDD/YYMM/YYYYMMDDhhmiss/hhmm)
 * @returns
 */
function getFormattedDateValue(value, formatType) {
	var formattedValue;
	try {
		if (formatType == 'YYYYMMDD') {
			var date = new ExDateTime().parseDate(value, false);
			formattedValue = ExDateTimeUtils.format(date, "Y/m/d");
		} else if(formatType == 'YYMM') {
			var date = new ExDateTime().parseYM(value);
			formattedValue = ExDateTimeUtils.format(date, "Y/m");
		} else if(formatType == 'YYYYMMDDhhmi') {
			var date = new ExDateTime().parseDate(value, true);
			formattedValue = ExDateTimeUtils.format(date, "Y/m/d H:i");
		} else if(formatType == 'YYYYMMDDhhmiss') {
			var date = new ExDateTime().parseDate(value, true);
			formattedValue = ExDateTimeUtils.format(date, "Y/m/d H:i:s");
		} else if(formatType == 'hhmm') {
			var date = new ExDateTime().parseTime(value);
			formattedValue = ExDateTimeUtils.format(date, "H:i");
		} else {
			return;
		}

		return formattedValue;
	} catch(e) {
			throw e;
	}
}

/**
 * 読取専用(disable)の項目に値を設定します。hidden値にも値が設定されます。
 * @param id ID
 * @param value 設定する値
 * @param isOverwrite 上書きするか
 */
function setDisabledValue(id, value, isOverwrite) {
	setValue(id, value, isOverwrite);
	setValue(id + "_disabled_hidden", value, isOverwrite);
}

/**
 * 数値フォーマットされたvalueを設定します。(text/checkbox/radio/hidden etc.)
 * @param id ID
 * @param value 設定する値
 * @param n 小数点以下桁数
 * @param isSeparate カンマ区切りするか
 * @param isOverwrite 上書きするか
 */
function setNumberFormatValue(id, value, n, isSeparate, isOverwrite) {
	var formattedNumber = "";
	if (!isNullorUndefinedorEmpty(value)) {
		formattedNumber = getFormatedNumberString(value, n, isSeparate);
	}
	setValue(id, formattedNumber, isOverwrite);
}

/**
 * 日付フォーマットされたvalueを設定します。(text/checkbox/radio/hidden etc.)
 *
 * @param id ID
 * @param value 設定する値
 * @param isOverwrite 上書きするか
 * @param formatType 日付フォーマットのタイプ(YYYYMMDD/YYMM/YYYYMMDDhhmiss/hhmm)
 */
function setDateFormatValue(id, value, isOverwrite, formatType) {
	var formattedDate = getFormattedDateValue(value, formatType);
	setValue(id, formattedDate, isOverwrite);
}

/**
 * ラベルに値を設定します。
 * ラベルの場合、hidden値にも値を設定します。
 *
 * @param id ID
 * @param value 設定する値
 */
function setLabelValue(id, value, isOverwrite) {
	setInnerTextValue(id, value, "_innerId", isOverwrite);
}

/**
 * ラベルに数値フォーマットされた値を設定します。
 *
 * @param id ID
 * @param value 設定する値
 * @param n 小数点以下桁数
 * @param isSeparate カンマ区切りするか
 * @param isOverwrite 上書きするか
 */
function setNumberFormatLabelValue(id, value, n, isSeparate, isOverwrite) {
	var formatNumber = getFormatedNumberString(value, n, isSeparate);
	setLabelValue(id, formatNumber, isOverwrite);
}

/**
 * ラベルに日付フォーマットされた値を設定します。
 *
 * @param id ID
 * @param value 設定する値
 * @param isOverwrite 上書きするか
 * @param formatType 日付フォーマットのタイプ(YYYYMMDD/YYMM/YYYYMMDDhhmiss/hhmm)
 */
function setDateFormatLabelValue(id, value, isOverwrite, formatType) {
	var formattedDate = getFormattedDateValue(value, formatType);
	setLabelValue(id, formattedDate, isOverwrite);
}

/**
 * Spanに値を設定します。
 * Spanの場合、hidden値にも値を設定します。
 * @param id ID
 * @param value 設定する値
 */
function setSpanValue(id, value, isOverwrite) {
	setInnerTextValue(id, value, "_innerId", isOverwrite);
}

/**
 * Spanに数値フォーマットされた値を設定します。
 * @param id ID
 * @param value 設定する値
 * @param n 小数点以下桁数
 * @param isSeparate カンマ区切りするか
 * @param isOverwrite 上書きするか
 */
function setNumberFormatSpanValue(id, value, n, isSeparate, isOverwrite) {
	var formatNumber = getFormatedNumberString(value, n, isSeparate);
	setSpanValue(id, formatNumber, isOverwrite);
}

/**
 * Spanに日付フォーマットされた値を設定します。
 *
 * @param id ID
 * @param value 設定する値
 * @param isOverwrite 上書きするか
 * @param formatType 日付フォーマットのタイプ(YYYYMMDD/YYMM/YYYYMMDDhhmiss/hhmm)
 */
function setDateFormatSpanValue(id, value, isOverwrite, formatType) {
	var formattedDate = getFormattedDateValue(value, formatType);
	setSpanValue(id, formattedDate, isOverwrite);
}

/**
 * Divに値を設定します。
 * Divの場合、hidden値にも値を設定します。
 * @param id ID
 * @param value 設定する値
 */
function setDivValue(id, value, isOverwrite) {
	setInnerTextValue(id, value, "_innerId", isOverwrite);
}

/**
 * Divに数値フォーマットされた値を設定します。
 * @param id ID
 * @param value 設定する値
 * @param n 小数点以下桁数
 * @param isSeparate カンマ区切りするか
 * @param isOverwrite 上書きするか
 */
function setNumberFormatDivValue(id, value, n, isSeparate, isOverwrite) {
	var formatNumber = getFormatedNumberString(value, n, isSeparate);
	setDivValue(id, formatNumber, isOverwrite);
}

/**
 * Divに日付フォーマットされた値を設定します。
 *
 * @param id ID
 * @param value 設定する値
 * @param isOverwrite 上書きするか
 * @param formatType 日付フォーマットのタイプ(YYYYMMDD/YYMM/YYYYMMDDhhmiss/hhmm)
 */
function setDateFormatDivValue(id, value, isOverwrite, formatType) {
	var formattedDate = getFormattedDateValue(value, formatType);
	setDivValue(id, formattedDate, isOverwrite);
}

/**
 * Preに値を設定します。
 * Preの場合、hidden値にも値を設定します。
 * @param id ID
 * @param value 設定する値
 */
function setPreValue(id, value, isOverwrite) {
	setInnerTextValue(id, value, "_innerId", isOverwrite);
}

/**
 * InnerTextに値を設定します。
 * hidden値にも同時に値を設定する必要のあるタグ項目(label/span/div/pre)の値を動的に変更するときに
 * 利用します。
 * ※ このメソッドは直接利用せず、setLabelValue メソッドなどを利用してください。
 * @param id ID
 * @param value 設定する値
 * @param kindId label/span/div/preなどを識別するID(WebSystemGeneratorの仕様に即します)
 */
function setInnerTextValue(id, value, kindId, isOverwrite) {
	setInnerText(id + kindId, value, isOverwrite);
	setValue(id, value, isOverwrite);
}

/**
 * ハッシュ値(主にJsonから取得するHashMap)からSelectBoxのOption値を構成します。
 * keysに取り出す順番にキーを格納することで順序を保つことができます。
 * @param selectbox SelectBox
 * @param hash ハッシュ値
 * @param defaultValue SelectBoxで設定する初期値
 * @param keys 値を取り出す順番のキーリスト
 */
function setOptionsFromHash(selectbox, hash, defaultValue, keys) {
	var inputSpaceMessage = getInputSpaceMessageFromSelectBox(selectbox);
	removeChildren(selectbox);

	if (hash != null && hash != undefined) {
		var optionIndex = 0;
		if (inputSpaceMessage != null) {
			selectbox.options[0] = new Option(inputSpaceMessage, "");
			optionIndex++;
		}
		if (keys != null && keys != undefined) {
			for (var index = 0; index < keys.length; index++) {
				selectbox.options[optionIndex] = new Option(hash[keys[index]], keys[index]);

				if (defaultValue != null && defaultValue != undefined) {
					if (keys[index] == defaultValue) {
						selectbox.options[optionIndex].selected = true;
					}
				}
				optionIndex++;
			}
		} else {
			for (var key in hash) {
				selectbox.options[optionIndex] = new Option(hash[key], key);

				if (defaultValue != null && defaultValue != undefined) {
					if (key == defaultValue) {
						selectbox.options[optionIndex].selected = true;
					}
				}
				optionIndex++;
			}
		}
	}
}

/**
 * SelectBoxのOption値をクリアします。
 * @param id アイテムID
 * @param indexSymbol インデックスシンボル
 */
function removeOptions(id, indexSymbol) {
	if (isNullorUndefined(indexSymbol)) {
		indexSymbol = "";
	}
	var itemElement = document.getElementById(id + indexSymbol);
	if (isNullorUndefined(itemElement)) {
		return;
	}

	var inputSpaceMessage = getInputSpaceMessageFromSelectBox(itemElement);
	removeChildren(itemElement);

	if (inputSpaceMessage != null) {
		itemElement.options[0] = new Option(inputSpaceMessage, "");
	}
}

/**
 * コンボボックスから未選択状態を表現する文字列を取得します。
 * 未選択状態を許可しないコンボボックスの場合、nullを返却します。
 * @param selectbox コンボボックス
 * @returns 未選択状態を表現する文字列。未選択状態を許可しない場合null
 */
function getInputSpaceMessageFromSelectBox(selectbox) {
	var inputSpaceMessage = "";
	if (!isNullorUndefined(selectbox) && selectbox.length > 0) {
		if (!isNullorUndefined(selectbox[0]) && !isNullorUndefined(selectbox[0].value) && selectbox[0].value == ""){
			isInputSpace = true;
			if (!isNullorUndefined(selectbox[0].innerText)) {
				inputSpaceMessage = selectbox[0].innerText;
			} else if (!isNullorUndefined(selectbox[0].textContent)) {
				// FireFox
				inputSpaceMessage = selectbox[0].textContent;
			}
			return inputSpaceMessage;
		}
	}
	return null;
}

/**
 * ハッシュ値(主にJsonから取得するHashMap)からRadioの選択値を構成します。
 * keysに取り出す順番にキーを格納することで順序を保つことができます。
 * @param name Radioのname属性
 * @param hash ハッシュ値
 * @param keys 値を取り出す順番のキーリスト
 * @param indexSymbol インデックスシンボル
 */
function setRadioSelectionsFromHash(name, hash, keys, indexSymbol) {
	setSelectionsFromHash(name, hash, keys, indexSymbol);
}

/**
 * ハッシュ値(主にJsonから取得するHashMap)からcheckboxの選択値を構成します。
 * keysに取り出す順番にキーを格納することで順序を保つことができます。
 * @param name checkboxのname属性
 * @param hash ハッシュ値
 * @param keys 値を取り出す順番のキーリスト
 * @param indexSymbol インデックスシンボル
 */
function setCheckboxSelectionsFromHash(name, hash, keys, indexSymbol) {
	setSelectionsFromHash(name, hash, keys, indexSymbol);
}

/**
 * ハッシュ値(主にJsonから取得するHashMap)から選択値を構成します。
 * keysに取り出す順番にキーを格納することで順序を保つことができます。
 * @param name name属性
 * @param hash ハッシュ値
 * @param keys 値を取り出す順番のキーリスト
 * @param indexSymbol インデックスシンボル
 */
function setSelectionsFromHash(name, hash, keys, indexSymbol) {
	removeSelectionsWithoutTemplate(name, indexSymbol);

	if (hash != null && hash != undefined) {
		if (keys != null && keys != undefined) {
			for (var index = 0; index < keys.length; index++) {
				addMultiShowSelectionFromTemplate(name, keys[index], hash[keys[index]], indexSymbol);
			}
		} else {
			for (var key in hash) {
				addMultiShowSelectionFromTemplate(name, key, hash[key], indexSymbol);
			}
		}
	}
}

/**
 * 指定したname属性を持つradioやcheckboxの選択値から未選択状態を表現する文字列を取得します。
 * 未選択状態を許可しない場合、nullを返却します。
 * @param name name属性
 * @param indexSymbol インデックスシンボル
 * @returns 未選択状態を表現する文字列。未選択状態を許可しない場合null
 */
function getInputSpaceMessageFromSelections(name, indexSymbol) {
	var inputSpaceMessage = "";

	var elements = document.getElementsByName(name + indexSymbol);
	if (isNullorUndefined(elements) || elements.length == 0) {
		return null;
	}

	for (var i = 0; i < elements.length; i++) {
		var element = elements[i];
		var elementId = element.id;
		if (isSelectionElementTemplate(elementId)) {
			continue;
		}
		var elementValue = getValueFromElement(element);
		if (elementValue == "") {
			var elementLabelId = elementId + "_label";
			var elementLabel = document.getElementById(elementLabelId);
			if (!isNullorUndefined(elementLabel)) {
				return getValueFromElement(elementLabel);
			}
		}
	}
	return null;
}

/**
 * 指定したname属性を持つradioやcheckboxの選択値をテンプレート以外削除します。
 * @param name name属性
 * @param indexSymbol インデックスシンボル
 */
function removeSelectionsWithoutTemplate(name, indexSymbol) {
	var elements = document.getElementsByName(name + indexSymbol);
	if (isNullorUndefined(elements) || elements.length == 0) {
		return;
	}
	var inputSpaceMessage = getInputSpaceMessageFromSelections(name, indexSymbol);
	var elementSize = elements.length;
	for (var i = 0; i < elementSize; i++) {
		var indexFromLast = elementSize - 1 - i;
		var element = elements[indexFromLast];
		if (isNullorUndefined(element)) {
			continue;
		}
		var elementId = element.id;
		if (isSelectionElementTemplate(elementId)) {
			// Templateは削除しない
			continue;
		}

		element.parentNode.removeChild(element);

		var elementLabelId = elementId + "_label";
		var elementLabel = document.getElementById(elementLabelId);
		elementLabel.parentNode.removeChild(elementLabel);
	}
	if (inputSpaceMessage != null) {
		addMultiShowSelectionFromTemplate(name, "", inputSpaceMessage, indexSymbol);
	}

}

/**
 * ダウンロード処理を実行します。
 * @param submitAction ダウンロードAction
 */
function downloadFile(submitAction) {
    var form = document.forms["formMain"];
    var screenUrl = form.action;
    var downloadUrl = screenUrl.replace("\.jsp", "\.download");
    form.action = downloadUrl;
    submit(submitAction, null, 2000, -1);
    form.action = screenUrl;
    return false;
}

/**
 * Gridsterによる編集モードをONにします。
 */
function gridEditOn() {
	if(gridster != null && gridster != undefined) {
		gridster.enable();
		$(".gridster > ul").removeClass("grid_edit_off");
		$(".gridster > ul").addClass("grid_edit_on");
	}
}

/**
 * Gridsterによる編集モードをOFFにします。
 */
function gridEditOff() {
	if(gridster != null && gridster != undefined) {
		gridster.disable();
		$(".gridster > ul").removeClass("grid_edit_on");
		$(".gridster > ul").addClass("grid_edit_off");
	}
}

/**
 * Gridレイアウト設定の読込み処理です。
 */
function initGridster(layoutPath){
	var widget_base_dimensions_width,widget_base_dimensions_height,widget_margins_x,widget_margins_y,min_cols,max_size_x;
	var s = $.ajaxSettings.async;
	$.ajaxSetup({async: false});
	$.post(layoutPath, function(json){
		if(json.list_item != null){
			for(i=0; i<json.list_item.length; i++){
				var listItemObj = $("#"+ json.list_item[i].block_name);
				listItemObj.attr("data-row",json.list_item[i].row);
				listItemObj.attr("data-col",json.list_item[i].col);
				listItemObj.attr("data-sizex",json.list_item[i].sizex);
				listItemObj.attr("data-sizey",json.list_item[i].sizey);
			}
		}
		widget_base_dimensions_width = json.widget_base_dimensions_width;
		widget_base_dimensions_height = json.widget_base_dimensions_height;
		widget_margins_x = json.widget_margins_x;
		widget_margins_y = json.widget_margins_y;
		min_cols = json.min_cols;
		max_size_x = json.max_size_x;
		gridster = $(".gridster > ul").gridster({
			widget_base_dimensions:[widget_base_dimensions_width,widget_base_dimensions_height],
			widget_margins:[widget_margins_x,widget_margins_y],
			min_cols:min_cols,
			max_size_x:max_size_x
		}).data('gridster');
	}, 'json');
	$.ajaxSetup({async: s});
}

/**
 * Gridレイアウト設定の保存処理です。
 */
function gridLayoutSave(layoutPath) {
	var form = document.forms["formMain"];
	var gridData = "{";
	var s = $.ajaxSettings.async;
	$.ajaxSetup({async: false});
	$.post(layoutPath, function(json){
		gridData += "\"widget_base_dimensions_width\":"+ json.widget_base_dimensions_width +",";
		gridData += "\"widget_base_dimensions_height\":"+ json.widget_base_dimensions_height +",";
		gridData += "\"widget_margins_x\":"+ json.widget_margins_x +",";
		gridData += "\"widget_margins_y\":"+ json.widget_margins_y +",";
		gridData += "\"min_cols\":"+ json.min_cols +",";
		gridData += "\"max_size_x\":"+ json.max_size_x +",";
		gridData += "\"list_item\":[";
		$(".gridster > ul > li").each(function(i) {
			gridData += (i == 0 ? "" : ",");
			gridData += "{";
			gridData += "\"block_name\":\""+ this.getAttribute("id") +"\",";
			gridData += "\"row\":"+ this.getAttribute("data-row") +",";
			gridData += "\"col\":"+ this.getAttribute("data-col") +",";
			gridData += "\"sizex\":"+ this.getAttribute("data-sizex") +",";
			gridData += "\"sizey\":"+ this.getAttribute("data-sizey") +"";
			gridData += "}";
		});
		gridData += "]";
	}, 'json');
	gridData += "}";
	$.ajaxSetup({async: s});
	form.elements["gridData"].value = gridData;
	disableCardInfo();
	form.submit();
}

/**
 * 明細行数を取得します。
 * @param detailsId 明細グループID
 * @returns 明細行数
 */
function getDataSize(detailsId) {
	return getIntValue(detailsId + "DataSize");
}

/**
 * 電話番号(3カラム)の入力チェックを実行します。
 * @param telId 電話番号(3カラム)の画面ID
 * @param isRequired 必須チェックを実施するか
 * @param indexSymbol インデックスシンボル
 */
function checkTel(telId, isRequired, indexSymbol) {
	if (isNullorUndefined(indexSymbol)) {
		indexSymbol = "";
	}
	var errorFieldBaseId = telId + indexSymbol;
	Validator.initbyId(errorFieldBaseId);

	var telNo = getTelValue(telId, indexSymbol);
	if (isNullorUndefinedorEmpty(telNo)) {
		if (isRequired) {
			Validator.outputbyId(errorFieldBaseId, Validator.rule.msg.noinput);
			return true;
		}
		return false;
	}

	return !Validator.checkTel(errorFieldBaseId, telNo);
}

/**
 * 電話番号(3カラム)の値を取得します。
 * @param telId 電話番号(3カラム)の画面ID
 * @param indexSymbol インデックスシンボル
 */
function getTelValue(telId, indexSymbol) {
	if (isNullorUndefined(indexSymbol)) {
		indexSymbol = "";
	}
	var idPre = telId + "Pre" + indexSymbol;
	var idMiddle = telId + "Middle" + indexSymbol;
	var idPost = telId + "Post" + indexSymbol;

	var idPreElement = document.getElementById(idPre);
	var idMiddleElement = document.getElementById(idMiddle);
	var idPostElement = document.getElementById(idPost);

	if (isNullorUndefined(idPreElement) && isNullorUndefined(idMiddleElement) && isNullorUndefined(idPostElement)) {
		return "";
	}

	var idPreValue = idPreElement.value;
	var idMiddleValue = idMiddleElement.value;
	var idPostValue = idPostElement.value;

	var isPreIdEmpty = isNullorUndefinedorEmpty(idPreValue);
	var isMiddleIdEmpty = isNullorUndefinedorEmpty(idMiddleValue);
	var isPostIdEmpty = isNullorUndefinedorEmpty(idPostValue);

	if (isPreIdEmpty && isMiddleIdEmpty && isPostIdEmpty) {
		return "";
	}

	var telNo = idPreValue + "-" + idMiddleValue + "-" + idPostValue;
	return telNo;
}

/**
 * 電話番号(3カラム)のカラムに値を設定します。
 * @param telId 電話番号(3カラム)の画面ID
 * @param value 設定値
 * @param isOverwrite 上書きするか
 * @param indexSymbol インデックスシンボル
 */
function setTelValue(telId, value, isOverwrite, indexSymbol) {
	if (isNullorUndefined(value)) {
		value = "";
	}

	if (isNullorUndefined(indexSymbol)) {
		indexSymbol = "";
	}

	var idPre = telId + "Pre" + indexSymbol;
	var idMiddle = telId + "Middle" + indexSymbol;
	var idPost = telId + "Post" + indexSymbol;

	var idPreElement = document.getElementById(idPre);
	var idMiddleElement = document.getElementById(idMiddle);
	var idPostElement = document.getElementById(idPost);

	if (isNullorUndefined(idPreElement)) {
		return false;
	}

	var idPreValue = idPreElement.value;
	var idMiddleValue = idMiddleElement.value;
	var idPostValue = idPostElement.value;

	var isPreIdEmpty = isNullorUndefinedorEmpty(idPreValue);
	var isMiddleIdEmpty = isNullorUndefinedorEmpty(idMiddleValue);
	var isPostIdEmpty = isNullorUndefinedorEmpty(idPostValue);

	if (!isPreIdEmpty || !isMiddleIdEmpty || !isPostIdEmpty) {
		if (!isOverwrite) {
			// 一つでも入力があったら上書きしない
			return false;
		}
	}

	if (value == "") {
		idPreElement.value = "";
		idMiddleElement.value = "";
		idPostElement.value = "";
	} else {
		var telValues = value.split("-");
		var telElements = new Array(3);
		telElements[0] = idPreElement;
		telElements[1] = idMiddleElement;
		telElements[2] = idPostElement;
		for (var i = 0; i < telValues.length; i++) {
			if (i > telElements.length - 1) {
				break;
			}
			telElements[i].value = telValues[i];
		}
	}
}

/**
 * 電話番号(3カラム)の入力が変更されたかを判定します。
 * @param telId 電話番号(3カラム)の画面ID
 * @param indexSymbol インデックスシンボル
 */
function isChangeTelObject(telId, indexSymbol) {
	if (isNullorUndefined(indexSymbol)) {
		indexSymbol = "";
	}
	var idPre = telId + "Pre" + indexSymbol;
	var idMiddle = telId + "Middle" + indexSymbol;
	var idPost = telId + "Post" + indexSymbol;

	var idPreElement = document.getElementById(idPre);
	var idMiddleElement = document.getElementById(idMiddle);
	var idPostElement = document.getElementById(idPost);

	if (isNullorUndefined(idPreElement)) {
		return false;
	}

	if (isChangeMultiPreAreaChangeFocusOutValue(idPreElement)) {
		return true;
	}

	if (isChangeMultiPreAreaChangeFocusOutValue(idMiddleElement)) {
		return true;
	}

	if (isChangeObject(idPostElement)) {
		return true;
	}

	return false;
}

/**
 * 郵便番号(2カラム)の入力チェックを実行します。
 * @param zipId 郵便番号(2カラム)の画面ID
 * @param isRequired 必須チェックを実施するか
 * @param indexSymbol インデックスシンボル
 */
function checkZip(zipId, isRequired, indexSymbol) {
	if (isNullorUndefined(indexSymbol)) {
		indexSymbol = "";
	}
	var errorFieldBaseId = zipId + indexSymbol;
	Validator.initbyId(errorFieldBaseId);

	var zipNo = getZipValue(zipId, indexSymbol);

	if (isNullorUndefinedorEmpty(zipNo)) {
		if (isRequired) {
			Validator.outputbyId(errorFieldBaseId, Validator.rule.msg.noinput);
			return true;
		}
		return false;
	}

	return !Validator.checkZip(errorFieldBaseId, zipNo);
}

/**
 * 郵便番号(2カラム)の値を取得します。
 * @param zipId 郵便番号(2カラム)の画面ID
 * @param indexSymbol インデックスシンボル
 */
function getZipValue(zipId, indexSymbol) {
	if (isNullorUndefined(indexSymbol)) {
		indexSymbol = "";
	}

	var idPre = zipId + "Pre" + indexSymbol;
	var idPost = zipId + "Post" + indexSymbol;

	var idPreElement = document.getElementById(idPre);
	var idPostElement = document.getElementById(idPost);
	if (isNullorUndefined(idPreElement) && isNullorUndefined(idPostElement)) {
		return "";
	}

	var idPreValue = idPreElement.value;
	var idPostValue = idPostElement.value;

	var isPreIdEmpty = isNullorUndefinedorEmpty(idPreValue);
	var isPostIdEmpty = isNullorUndefinedorEmpty(idPostValue);

	if (isPreIdEmpty && isPostIdEmpty) {
		return "";
	}
	var zipNo = idPreValue + "-" + idPostValue;
	return zipNo;
}

/**
 * 郵便番号(2カラム)のカラムに値を設定します。
 * @param zipId 郵便番号(2カラム)の画面ID
 * @param value 設定値
 * @param isOverwrite 上書きするか
 * @param indexSymbol インデックスシンボル
 */
function setZipValue(zipId, value, isOverwrite, indexSymbol) {
	if (isNullorUndefined(value)) {
		value = "";
	}

	if (isNullorUndefined(indexSymbol)) {
		indexSymbol = "";
	}

	var idPre = zipId + "Pre" + indexSymbol;
	var idPost = zipId + "Post" + indexSymbol;

	var idPreElement = document.getElementById(idPre);
	var idPostElement = document.getElementById(idPost);

	if (isNullorUndefined(idPreElement)) {
		return false;
	}

	var idPreValue = idPreElement.value;
	var idPostValue = idPostElement.value;

	var isPreIdEmpty = isNullorUndefinedorEmpty(idPreValue);
	var isPostIdEmpty = isNullorUndefinedorEmpty(idPostValue);

	if (!isPreIdEmpty || !isPostIdEmpty) {
		if (!isOverwrite) {
			// 一つでも入力があったら上書きしない
			return false;
		}
	}

	if (value == "") {
		idPreElement.value = "";
		idPostElement.value = "";
	} else {
		var zipValues = value.split("-");
		var zipElements = new Array(2);
		zipElements[0] = idPreElement;
		zipElements[1] = idPostElement;
		for (var i = 0; i < zipValues.length; i++) {
			if (i > zipElements.length - 1) {
				break;
			}
			zipElements[i].value = zipValues[i];
		}
	}
}

/**
 * 郵便番号(2カラム)の入力が変更されたかを判定します。
 * @param zipId 郵便番号(2カラム)の画面ID
 * @param indexSymbol インデックスシンボル
 */
function isChangeZipObject(zipId, indexSymbol) {
	if (isNullorUndefined(indexSymbol)) {
		indexSymbol = "";
	}
	var idPre = zipId + "Pre" + indexSymbol;
	var idPost = zipId + "Post" + indexSymbol;

	var idPreElement = document.getElementById(idPre);
	var idPostElement = document.getElementById(idPost);

	if (isNullorUndefined(idPreElement)) {
		return false;
	}

	if (isChangeMultiPreAreaChangeFocusOutValue(idPreElement)) {
		return true;
	}

	if (isChangeObject(idPostElement)) {
		return true;
	}

	return false;
}

/**
 * 指定されたタブオブジェクトのタブ選択を次のタブに移動します。最後のタブにいる場合は、先頭のタブに移動します。
 * @param tab タブオブジェクト
 */
function changeSelectedTabNext(tab) {
	var active = tab.tabs("option", "active");
	var next = active + 1;
	changeSelectedTab(tab, next, 0);
}

/**
 * 指定されたタブオブジェクトのタブ選択を前のタブに移動します。最初のタブにいる場合は、最後のタブに移動します。
 * @param tab タブオブジェクト
 */
function changeSelectedTabPrev(tab) {
	var active = tab.tabs("option", "active");
	var prev = active - 1;
	changeSelectedTab(tab, prev);
}

/**
 * 指定されたタブオブジェクトのタブ選択を指定された位置に移動します。
 * @param tab タブオブジェクト
 * @param moveIndex 移動するタブインデックス
 * @param moveIfNotMoveIndex 指定されたインデックスに移動できなかった場合に移動するタブインデックス
 */
function changeSelectedTab(tab, moveIndex, moveIfNotMoveIndex) {
	tab.tabs('option', 'active', moveIndex);
	var nowActive = tab.tabs("option", "active");
	if (!isNullorUndefined(moveIfNotMoveIndex)) {
		if (nowActive != moveIndex) {
			tab.tabs('option', 'active', moveIfNotMoveIndex);
		}
	}
}

/**
 * タブをアクティブにした際にTabをタブパネルの先頭のフォーカスに移動します。
 * @param e アクティブ時のイベント
 * @param ui Panel UserInterface
 */
function focusEnabledActivateTabFirst(e, ui) {
	if (isNullorUndefined(ui) || isNullorUndefined(ui.newPanel)) {
		return;
	}
	if (ui.newPanel.length == 0) {
		return;
	}

	focusEnabledFirst(ui.newPanel[0].getElementsByTagName("*"));
}

/**
 * インデックスシンボルから親明細が存在するかを判断します。
 * @param indexSymbol インデックスシンボル
 * @returns 親明細が存在するかどうか
 */
function isExistParentDetails(indexSymbol) {
	if (isNullorUndefinedorEmpty(indexSymbol)) {
		return false;
	}
	return indexSymbol.match(/.*_\d+_\d+$/);
}

/**
 * 親明細のインデックスシンボルを取得します。
 * @param indexSymbol インデックスシンボル
 * @param hierarchy 親をたどる回数(省略可) 1または省略時は、1階層親のインデックスシンボルを返却。2なら2階層親のインデックスシンボルを返却。
 * @returns 親明細のインデックスシンボル
 */
function getParentIndexSymbol(indexSymbol, hierarchy) {
	if (isNullorUndefinedorEmpty(indexSymbol)) {
		return "";
	}

	if (isNullorUndefinedorEmpty(hierarchy) || hierarchy == 1) {
		// 階層指定なしまたは1階層指定なら1つ親を返す
		return indexSymbol.replace(/_\d+$/, "");
	} else if (hierarchy < 1) {
		// 階層指定が1未満ならそのまま返す
		return indexSymbol;
	}

	// 階層が1になるまで再起呼び出し
	return getParentIndexSymbol(indexSymbol.replace(/_\d+$/, ""), hierarchy - 1);
}

/**
 * インデックスシンボルから選択行を取得します。
 * @param indexSymbol インデックスシンボル
 * @returns 選択行
 */
function getRowNumberFromIndexSymbol(indexSymbol) {
	if (isNullorUndefinedorEmpty(indexSymbol)) {
		return "";
	}
	return indexSymbol.replace(/.*_/, "");
}

/**
 * 指定されたIDのタブに移動します。
 * @param tabId 移動先タブのID
 * @param isFocusFirstElement タブ選択時に先頭項目にタブを移動するか
 */
function selectTabById(tabId, isFocusFirstElement) {
	var tab = $("#" + tabId + "Link");
	if (!tab != null) {
		var tabGroup = tab.closest("div");
		if (tabGroup != null) {
			var index = tab.attr("index");
			if (index != null) {
				changeSelectedTab(tabGroup, index);
				if (isNullorUndefined(isFocusFirstElement) || isFocusFirstElement) {
					focusEnabledFirst(document.getElementById(tabId + "Tab").getElementsByTagName("*"));
				}
			}
		}
	}
}

/**
 * 子ウィンドウとしての振る舞いをコントロールします。(主にIE対策で利用します)
 */
function childWindowControl() {
	var childWindowName = "_child";
    window.name = childWindowName
    var form = document.forms["formMain"];
    if (!isNullorUndefined(form)) {
	    form.target = childWindowName;
	}
}

/**
 * 待機表示中のダイアログを表示します。
 */
function showWaitDialog() {
	var waitDialog = getWaitDialog();
	if (!isNullorUndefined(waitDialog)) {
		waitDialog.style.display = "block";
	}
}

/**
 * 待機表示中のダイアログを隠します。
 */
function hideWaitDialog() {
	var waitDialog = getWaitDialog();
	if (!isNullorUndefined(waitDialog)) {
		waitDialog.style.display = "none";
	}
}

/**
 * 待機表示中のダイアログを取得します。
 */
function getWaitDialog() {
	return document.getElementById("wait_dialog");
}

/**
 * datepickerの設定を行います。
 * @param itemId 項目ID
 * @param isDateYm trueの場合はYYYY/MM形式、falseの場合はYYYY/MM/DD形式
 * @returns {Boolean}
 */
function settingDatepicker(itemId, isDateYm) {
	var datepickerItem = $("#" + itemId);
	if (isNullorUndefined(datepickerItem)) {
		return false;
	}
	if (isNullorUndefined(datepickerItem.datepicker)) {
		 return false;
	}
	datepickerItem.datepicker({
		beforeShow: function(selectedDate) {
			var obj = document.getElementById(itemId);
			obj.datepickerInFocusValue = selectedDate.value;
			obj.isChangeScriptLock = true;
			obj.isChangeByScript = false;
			var readOnly = $("#" + selectedDate.id).attr('readOnly');
			if (!isNullorUndefined(readOnly) && 0 < readOnly.length) {
				return false;
			}
		},
		onClose : function(value) {
			var obj = document.getElementById(itemId);
			obj.isChangeScriptLock = false;
			obj.focus();
			if (obj.datepickerInFocusValue != value) {
				obj.isChangeByScript = true;
			} else {
				obj.isChangeByScript = false;
			}
		},
		showOtherMonths: true,
		selectOtherMonths: true,
		showButtonPanel: true,
		changeMonth: true,
		changeYear: true
	});
	if (isDateYm) {
		datepickerItem.datepicker("option", "dateFormat", "yy/mm");
	}
}

/**
 * datetimepickerの設定を行います。
 * @param itemId 項目ID
 * @param isTimestamp trueの場合はミリ秒まで出力、falseの場合は秒まで
 * @returns {Boolean}
 */
function settingDatetimepicker(itemId, isTimestamp) {
	var datepickerItem = $("#" + itemId);
	if (isNullorUndefined(datepickerItem)) {
		return false;
	}
	if (isNullorUndefined(datepickerItem.datetimepicker)) {
		 return false;
	}
	if (isTimestamp) {
		datepickerItem.datetimepicker({
			beforeShow: function(selectedDate) {
				var obj = document.getElementById(itemId);
				obj.datepickerInFocusValue = selectedDate.value;
				obj.isChangeScriptLock = true;
				obj.isChangeByScript = false;
				var readOnly = $("#" + selectedDate.id).attr('readOnly');
				if (!isNullorUndefined(readOnly) && 0 < readOnly.length) {
					return false;
				}
			},
			onClose : function(value) {
				var obj = document.getElementById(itemId);
				obj.isChangeScriptLock = false;
				obj.focus();
				if (obj.datepickerInFocusValue != value) {
					obj.isChangeByScript = true;
				} else {
					obj.isChangeByScript = false;
				}
			},
			showOtherMonths: true,
			selectOtherMonths: true,
			showSecond: true,
			showMillisec: true,
			timeFormat: 'HH:mm:ss l'
		});
	} else {
		datepickerItem.datetimepicker({
			beforeShow: function(selectedDate) {
				var obj = document.getElementById(itemId);
				obj.datepickerInFocusValue = selectedDate.value;
				obj.isChangeScriptLock = true;
				obj.isChangeByScript = false;
				var readOnly = $("#" + selectedDate.id).attr('readOnly');
				if (!isNullorUndefined(readOnly) && 0 < readOnly.length) {
					return false;
				}
			},
			onClose : function(value) {
				var obj = document.getElementById(itemId);
				obj.isChangeScriptLock = false;
				obj.focus();
				if (obj.datepickerInFocusValue != value) {
					obj.isChangeByScript = true;
				} else {
					obj.isChangeByScript = false;
				}
			},
			showOtherMonths: true,
			selectOtherMonths: true,
			showSecond: true,
			timeFormat: 'HH:mm:ss',
			changeMonth: true,
			changeYear: true
		});
	}
}

/**
 * datetimepickerの設定を行います。(年月日 時分)
 * @param itemId 項目ID
 * @returns {Boolean}
 */
function settingDateHourMinutestimepicker(itemId, isTimestamp) {
	var datepickerItem = $("#" + itemId);
	if (isNullorUndefined(datepickerItem)) {
		return false;
	}
	if (isNullorUndefined(datepickerItem.datetimepicker)) {
		 return false;
	}
	datepickerItem.datetimepicker({
		beforeShow: function(selectedDate) {
			var obj = document.getElementById(itemId);
			obj.datepickerInFocusValue = selectedDate.value;
			obj.isChangeScriptLock = true;
			obj.isChangeByScript = false;
			var readOnly = $("#" + selectedDate.id).attr('readOnly');
			if (!isNullorUndefined(readOnly) && 0 < readOnly.length) {
				return false;
			}
		},
		onClose : function(value) {
			var obj = document.getElementById(itemId);
			obj.isChangeScriptLock = false;
			obj.focus();
			if (obj.datepickerInFocusValue != value) {
				obj.isChangeByScript = true;
			} else {
				obj.isChangeByScript = false;
			}
		},
		showOtherMonths: true,
		selectOtherMonths: true,
		showSecond: false,
		showMillisec: false,
		timeFormat: 'HH:mm'
	});
}

/**
 * timepickerの設定を行います。
 * @param itemId 項目ID
 * @returns {Boolean}
 */
function settingTimepicker(itemId) {
	var datepickerItem = $("#" + itemId);
	if (isNullorUndefined(datepickerItem)) {
		return false;
	}
	if (isNullorUndefined(datepickerItem.timepicker)) {
		 return false;
	}
	datepickerItem.timepicker({
		beforeShow: function(selectedDate) {
			var obj = document.getElementById(itemId);
			obj.datepickerInFocusValue = selectedDate.value;
			obj.isChangeScriptLock = true;
			obj.isChangeByScript = false;
			var readOnly = $("#" + selectedDate.id).attr('readOnly');
			if (!isNullorUndefined(readOnly) && 0 < readOnly.length) {
				return false;
			}
		},
		onClose : function(value) {
			var obj = document.getElementById(itemId);
			obj.isChangeScriptLock = false;
			obj.focus();
			if (obj.datepickerInFocusValue != value) {
				obj.isChangeByScript = true;
			} else {
				obj.isChangeByScript = false;
			}
		},
		showOtherMonths: true,
		selectOtherMonths: true,
		showSecond: true,
		timeFormat: 'HH:mm:ss'
	});
}

/**
 * 指定された文字数入力された場合、次のエレメントへフォーカスを移動させます。
 * @param obj 対象エレメント
 * @param e keyupイベント
 * @param maxLength 何文字入れた場合に、次のフォーカスに移動するか
 * @param nextFocusId 次のフォーカス先エレメントID
 */
function onkeyupToNextFocus(obj, e, maxLength, nextFocusId) {
	if (isNullorUndefined(obj)) {
		return true;
	}

	if (!isChangeObject(obj)) {
		return true;
	}

	if (obj.value.length == maxLength) {
		// フォーカス移動
		var nextElement = document.getElementById(nextFocusId);
		if (!isNullorUndefined(nextElement)) {
			nextElement.focus();
		}
	}
	return true;
}

/**
 * radioや複数checkboxのような複数選択肢がある項目に選択項目を追加します。<br/>
 * 選択肢のコピー元となるtemplateがJSPに出力されていることが前提になります。
 * @param name input name
 * @param key 選択肢のkey値
 * @param value 選択肢の表示値
 * @param indexSymbol インデックスシンボル
 * @param elementType エレメントタイプ(checkbox/radio)
 */
function addMultiShowSelectionFromTemplate(name, key, value, indexSymbol) {
	var templateParentElement = document.getElementById(name + indexSymbol + "_template");
	if (isNullorUndefined(templateParentElement)) {
		return;
	}

	var elements = document.getElementsByName(name + indexSymbol);
	if (isNullorUndefined(elements) || elements.length == 0) {
		return;
	}

	var nowMaxElementIndex = -1;
	for (var i = 0; i < elements.length; i++) {
		var element = elements[i];
		var elementId = element.id;
		if (isSelectionElementTemplate(elementId)) {
			continue;
		}

		var elementSplits = elementId.split("_");
		var elementIdIndex = elementSplits[elementSplits.length - 1];
		if (elementIdIndex == getSelectionElementTemplateIndex()) {
			continue;
		}

		var elementIndexNumber = -1;
		try {
			elementIndexNumber = Number(elementIdIndex);
		} catch(e) {
			continue;
		}
		if (elementIndexNumber > nowMaxElementIndex) {
			nowMaxElementIndex = elementIndexNumber;
		}
	}
	var addIndex = nowMaxElementIndex + 1;

	var nowHtml = templateParentElement.parentNode.innerHTML;
	var addHtml = templateParentElement.innerHTML;
	addHtml = addHtml.replace(/\{template\}/g, addIndex);
	templateParentElement.parentNode.innerHTML = nowHtml + addHtml;

	var addElementId = name + indexSymbol + "_" + addIndex;
	setValue(addElementId, key, true);

	var addLableElementId = addElementId + "_label";
	var labelElement = document.getElementById(addLableElementId);
	var value = labelElement.innerHTML + value;
	labelElement.innerHTML = value;
}

/**
 * 対象ElementのIDがRadioやcheckbox選択値のテンプレートIDであるか判定します。
 * @param elementId ElementのID
 * @returns 対象ElementのIDが選択値のテンプレートIDである場合true
 */
function isSelectionElementTemplate(elementId) {
	if (elementId.lastIndexOf("_" + getSelectionElementTemplateIndex())  > 0) {
		return true;
	} else {
		return false;
	}
}

/**
 * Radioやcheckbox選択値のテンプレートを示すIndex文字列を返却します。
 * @returns Radioやcheckbox選択値のテンプレートを示すIndex文字列
 */
function getSelectionElementTemplateIndex() {
	return "{template}";
}

/**
 * バッチ実行の画面状況を更新します。
 * @param batchId バッチID
 * @param executeTitle バッチ状況表示の主語(タイトル)
 * @param executeResult バッチ実行状況を保持する領域のID
 * @param batchExecuteId バッチ実行を特定する実行IDを保持する領域のID
 * @param executeStatus バッチ実行ステータスを表示する領域のID
 * @param executeProgress バッチ実行進捗度を保持する領域のID
 * @param loadingAreaId ロードイメージを表示する領域のID(定義されていない場合、executeStatusの直前に表示)
 */
function updateBatchExecuteStatus(batchId, executeTitle, executeResult, batchExecuteId, executeStatus, executeProgress, loadingAreaId) {
	if (!isNullorUndefinedorEmpty(loadingAreaId)) {
		hideLoadingArea(loadingAreaId);
	}
	var executeResultValue = getElementValue(executeResult);
	var progressBarId = executeProgress + "ProgressBarShowArea";
	var progress = getElementValue(executeProgress);
	changeBatchExecuteStatus(batchId, executeTitle, executeResultValue, progress, executeStatus, loadingAreaId, progressBarId);
	if (batchExecuteResultIsInprogress(executeResultValue)) {
	    var paramBatchExecuteId = getElementValue(batchExecuteId);
		// 状況確認Ajax実行
        $.ajaxSetup({ cache: false });
		var timerId = setInterval(function() {
	        $.getJSON("jp/eltex/afw/eg/batch/frontif/bean/ajax/AjaxBatchExecuteResultConfirmBean.json", {"batchId":batchId, "batchExecuteId":paramBatchExecuteId}, function(info) {
	            var nowExecuteResult = info.executeResult;
	            var inProgress = batchExecuteResultIsInprogress(nowExecuteResult);
	            if (inProgress) {
		            if (!isNullorUndefinedorEmpty(info.executeProgress)) {
		            	if (progress != info.executeProgress) {
			            	progress = info.executeProgress;
			            	// 実行中の場合、進捗度更新時のみ更新
							changeBatchExecuteStatus(batchId, executeTitle, nowExecuteResult, info.executeProgress, executeStatus, loadingAreaId, progressBarId);
						}
					}
	            } else {
					changeBatchExecuteStatus(batchId, executeTitle, nowExecuteResult, info.executeProgress, executeStatus, loadingAreaId, progressBarId);
					clearInterval(timerId);
					callbackBatchExecuteDone(batchId, executeTitle, nowExecuteResult);
					return;
	            }
			});
		}, 2000);
	}
}

/**
 * バッチ画面状況をバッチ実行状況によって変更します。
 * @param batchId バッチID
 * @param executeTitle バッチ状況表示の主語(タイトル)
 * @param executeResult バッチ実行状況の値
 * @param progress バッチ実行進捗度の値
 * @param executeStatus バッチ実行ステータスを表示する領域のID
 * @param loadingAreaId ロードイメージを表示する領域のID
 * @param progressBarId プログレスバーのID(バッチ実行ステータスを表示する領域の後ろにプログレスバーが表示されます)
 */
function changeBatchExecuteStatus(batchId, executeTitle, executeResult, progress, executeStatus, loadingAreaId, progressBarId) {
	var linkText = "..";
	if (batchExecuteResultIsInprogress(executeResult)) {
		if (!isNullorUndefinedorEmpty(loadingAreaId)) {
			showLoadingArea(loadingAreaId);
		} else {
			// ロードイメージエリアをexecuteStatusの手前に挿入
			createLoadingArea(executeStatus);
		}

		linkText = executeTitle + "実行中です..";
		if (!isNullorUndefinedorEmpty(progress)) {
			// プログレスバー表示
			createAndUpdateBatchExecuteProgress(executeStatus, progressBarId, Math.floor(progress));
		}

		callbackBatchStatusInprogress(batchId, executeTitle);
	} else {
		if (!isNullorUndefinedorEmpty(loadingAreaId)) {
			hideLoadingArea(loadingAreaId);
		} else {
			// 挿入されたロードイメージエリアを削除
			removeLoadingArea(executeStatus);
		}
		if (executeResult == "2") {
			linkText = executeTitle + "実行可能です[！前回実行時にエラーが発生しています]";
		} else if (executeResult == "3") {
			linkText = executeTitle + "バッチメンテナンス中のため実行できません";
		} else {
			linkText = executeTitle + "実行可能です";
		}
		// プログレスバー非表示
		removeBatchExecuteProgress(progressBarId);

		callbackBatchStatusEnd(batchId, executeTitle, executeResult);
	}

	$("#" + executeStatus).text(linkText);
}

/**
 * 入金確定バッチが実行中か返却します。
 * @param executeResult バッチ実行状況
 * @returns バッチが実行中の場合true
 */
function batchExecuteResultIsInprogress(executeResult) {
	if (executeResult == "4") {
		return true;
	} else {
		return false;
	}
}

/**
 * ローディングイメージを表示します。
 * @param loadingAreaId ロードイメージを表示する領域のID
 */
function showLoadingArea(loadingAreaId) {
	showArea(loadingAreaId);
}

/**
 * ローディングイメージを隠します。
 * @param loadingAreaId ロードイメージを表示する領域のID
 */
function hideLoadingArea(loadingAreaId) {
	hideArea(loadingAreaId);
}

/**
 * ローディングイメージを実行ステータス表示領域の手前に挿入します。
 * @param executeStatus 表示領域のID
 */
function createLoadingArea(executeStatus) {
	var loadingAreaId = makeCreateLoadingAreaId(executeStatus);
	var loadingArea = $("#" + loadingAreaId);
	if (loadingArea.size() != 0) {
		// 既に表示済み
		return;
	}

	var executeStatusArea = $("#" + executeStatus);
	if (executeStatusArea.size() == 0) {
		// 表示エリアがないので、ローディングイメージも表示させない
		return;
	}

	var loadingAreaId = makeCreateLoadingAreaId(executeStatus);
	var loadingAreaDiv = '<div id="' + loadingAreaId +  '" class="loadingImg"></div>';
	$(loadingAreaDiv).insertBefore("#" + executeStatus);
}

/**
 * ローディングイメージを実行ステータス表示領域から削除します。
 * @param executeStatus 表示領域のID
 */
function removeLoadingArea(executeStatus) {
	var loadingImage = $("#" + makeCreateLoadingAreaId(executeStatus));
	if (loadingImage != null) {
		loadingImage.remove();
	}
}

/**
 * ローディングイメージのIDを実行ステータス表示領域のIDから作成します。
 * @param executeStatus 表示領域のID
 */
function makeCreateLoadingAreaId(executeStatus) {
	var executeStatusArea = $("#" + executeStatus);
	return executeStatusArea.attr("id") + "LoadingArea";
}

/**
 * プログレスバーを表示します。executeStatus領域の後ろに表示されます。
 * @param executeStatus プログレスバーを後ろに表示するステータス領域
 * @param progressBarId プログレスバーのID
 * @param progress 進捗度
 */
function createAndUpdateBatchExecuteProgress(executeStatus, progressBarId, progress) {
	var executeStatusArea = $("#" + executeStatus);
	if (executeStatusArea.size() == 0) {
		// 表示エリアがないので、プログレスバーも表示させない
		return;
	}

	var progressTextId = progressBarId + "Text";

	var progressBar = $("#" + progressBarId);
	if (progressBar.size() == 0) {
		// 存在しないので作成
		// プログレスバーはdivの入れ子で表現される .. <div id="[progressBarId]"><div id="[progressTextId]"></div></div>
		var progressDiv = '<div id="' + progressBarId +  '" class="progressBar">' + '<div id="' + progressTextId +  '" class="progressBarText">' + '</div>' + '</div>'
		$(progressDiv).insertAfter("#" + executeStatus);
		if (progress == 0) {
			progress = false;
		} else {
			$("#" + progressTextId).text(progress + "%");
			progress = parseInt(progress);
		}
		$("#" + progressBarId).progressbar({
			value: progress,
			max: 100
		});

	} else {
		// 値の更新
		if (progress != 0) {
			progressBar.progressbar("value", progress);
			$("#" + progressTextId).text(progress + "%");
		}
	}
}

/**
 * プログレスバーを非表示にします。
 * @param progressBarId プログレスバーのID
 */
function removeBatchExecuteProgress(progressBarId) {
	var progressBar = $("#" + progressBarId);
	if (progressBar != null) {
		progressBar.remove();
	}
}

/**
 * バッチ実行のステータスが完了している際に呼ばれるコールバックメソッドです。
 * 必要に応じて、各画面でメソッドを上書きしてください。
 * @param batchId 実行したBatchID
 * @param executeTitle 実行のタイトル
 * @param executeResult Batch実行結果(1: 正常終了、2: 異常終了、3: メンテナンス中)
 */
function callbackBatchStatusEnd(batchId, executeTitle, executeResult) {

}

/**
 * バッチ実行のステータスが実行中の場合に呼ばれるコールバックメソッドです。
 * 必要に応じて、各画面でメソッドを上書きしてください。
 * @param batchId 実行したBatchID
 * @param executeTitle 実行のタイトル
 */
function callbackBatchStatusInprogress(batchId, executeTitle) {

}

/**
 * バッチ実行完了時に呼ばれるコールバックメソッドです。(バッチが実際に実行されていた場合に呼ばれます)
 * 現実装では、空のサブミットを実施しますが、必要に応じて、各画面でメソッドを上書きしてください。
 * @param batchId 実行したBatchID
 * @param executeTitle 実行のタイトル
 * @param executeResult Batch実行結果(1: 正常終了、2: 異常終了、3: メンテナンス中)
 */
function callbackBatchExecuteDone(batchId, executeTitle, executeResult) {
	submit(null);
}

/**
 * キーイベントを無視するかどうかを判定します。
 * @param obj イベント発生オブジェクト
 * @returns {Boolean} true : 無視する
 */
function isIgnoreOnkeydownEvent(obj) {
	if (obj.type == 'text' || obj.type == 'radio' || obj.type == 'checkbox' || obj.type == 'select-one' || obj.type == 'select-multiple' || obj.type == 'textarea' ||  obj.type == 'password') {
		return true;
	} else {
		return false;
	}
}

/**
 * 入力チェックエラー時に呼ばれる関数です。
 */
function existErrorAction() {
	Validator.focusToFirstErrorElement();
	existErrorActionContents();
}

/**
 * 入力チェックエラー時に呼ばれる関数です。
 * 必要に応じて、各画面ないし共通JSで上書きしてください。
 */
function existErrorActionContents() {

}

/**
 * 明細番号リンクにフォーカスさせます。
 * @param listId 明細リストID
 * @param rowIndex 行インデックス
 * @param indexSymbol インデックスシンボル
 * @param isForceFocus 明細内にフォーカスされたエレメントがあったとしても強制的に明細番号にフォーカスを移動させるか
 * @returns 移動した場合true
 */
function focusToListSequence(listId, rowIndex, indexSymbol, isForceFocus) {
    var targetSequenceElement = document.getElementById(listId + "Sequence" + indexSymbol);
    if (isNullorUndefined(targetSequenceElement)) {
    	return false;
    }

    var isFocus = false;
    if (isForceFocus) {
    	isFocus = true;
    } else {
	    var activeElement = document.activeElement;
	    if (isNullorUndefined(activeElement)) {
	        return false;
	    }
	    var activeElementId = activeElement.id;
	    if (isNullorUndefined(activeElementId)) {
	        return false;
	    }
	    if (activeElementId.indexOf(indexSymbol, activeElementId.length - indexSymbol.length) == -1) {
		    isFocus = true;
	    }
    }

    if (isFocus) {
	    targetSequenceElement.focus();
	    return true;
	} else {
	    return false;
	}
}


/**
 * 引数URLにContextPathをつけたURLを返却します。
 * @param url 指定URL(先頭の/は削除して渡すこと)
 * @param relativePath 相対パス(コンテキストパスが設定されていれば不要)
 * @returns 引数URLにContextPathをつけたURL
 */
function getUrlWithContextPath(url, relativePath) {
	var contextPathId = "appContextPath";
	var contextPathElement = document.getElementById(contextPathId);
	if (isNullorUndefined(contextPathElement)) {
		// 設定がないので、相対パスでコンテキストパスを辿る
		return relativePath + url;
	}

 	var contextPath = getElementValue(contextPathId);
	var urlWithContextPath;
	if (contextPath == "" || contextPath == "/") {
		urlWithContextPath = "/" + url;
	} else {
		urlWithContextPath = contextPath + url;
	}
	return urlWithContextPath;
}

/**
 * 指定したアイテムへフォーカスを移動します。<br/>
 * タブで隠れているアイテムの場合、アイテムが存在するタブを選択します。
 * @param itemId 対象アイテムID
 * @returns フォーカスしたかどうか
 */
function focusToItem(itemId) {
	var item = document.getElementById(itemId);
	if (isNullorUndefined(item)) {
		// 電話番号、郵便番号の可能性
		var originalItemId = itemId.split("_")[0];
		var tempId = itemId.replace(new RegExp("^" + originalItemId), originalItemId + "Pre");
		var tempItem = document.getElementById(tempId);
		if (!isNullorUndefined(tempItem)) {
			itemId = tempId;
			item = tempItem;
		} else {
			// radio/checkboxの可能性
			tempId = itemId + "_0";
			tempItem = document.getElementById(tempId);
			var type = tempItem.type;
			if (type == 'radio' || type == 'checkbox') {
				itemId = tempId;
				item = tempItem;
			} else {
				return false;
			}
		}
	}

	// 対象アイテムが所属するタブへ移動(タブがあれば)
	moveTabToItemExist(itemId, false);

	if (isTargetItemVisible(itemId)) {
		// フォーカス移動の表現
		moveScrollToTargetItem(itemId);

		// フォーカス移動
		if (item.focus != undefined) {
			item.focus();
		}
		return true;
	} else {
		focusToItemIfItemInvisible(itemId);
	}
}

/**
 * 対象アイテムが存在するタブに移動します。<br/>
 * タブが階層化されている場合にも対応しています。
 * @param itemId 対象アイテムID
 * @param isFocusFirstElement タブ選択時に先頭項目にタブを移動するか
 */
function moveTabToItemExist(itemId, isFocusFirstElement) {
	var item = document.getElementById(itemId);
	if (isNullorUndefined(item)) {
		return;
	}

	var parentTabId = getTabIdBelongItem(item);
	if (!isNullorUndefinedorEmpty(parentTabId)) {
		var moveTabPanelId = parentTabId.replace(/Tab$/, "");
		selectTabById(moveTabPanelId, isFocusFirstElement);
		var moveTabPanel = document.getElementById(moveTabPanelId);
		if (!isNullorUndefined(moveTabPanel) && !isNullorUndefined(moveTabPanel.parentNode)) {
			var moveTabPanelParent = moveTabPanel.parentNode.parentNode;
			if (!isNullorUndefined(moveTabPanelParent)) {
				moveTabToItemExist(moveTabPanelParent.id, isFocusFirstElement);
			}
		}
	}

}

/**
 * 対象アイテムが所属するタブIDを取得します。
 * @param itemId アイテムID
 * @returns 対象アイテムが所属するタブID
 */
function getTabIdBelongItem(element) {
	if (isNullorUndefined(element)) {
		return null;
	}

	if (isNullorUndefined(element.getAttribute)) {
		return null;
	}

	var attributeRole = element.getAttribute("role");
	if (!isNullorUndefined(attributeRole)) {
		if (attributeRole == "tabpanel") {
			return element.id;
		}
	} else {
		if (isNullorUndefined(element.parentNode)) {
			return null;
		} else {
			return getTabIdBelongItem(element.parentNode);
		}
	}
}

/**
 * 対象アイテム項目へアイテムIDを指定してスクロールを移動します。
 * @param itemId 対象アイテムID
 */
function moveScrollToTargetItem(itemId) {
	var itemPosition = $("#" + itemId).offset().top - 20;
	moveScrollToTargetPosition(itemPosition);
}

/**
 * 対象ポジションへスクロールを移動します。
 * @param item 対象アイテム(ただしjQueryのオブジェクトであること)
 */
function moveScrollToTargetPosition(itemPosition) {
	if (itemPosition < 0) {
		itemPosition = 0;
	}
	$('html,body').animate({ scrollTop: itemPosition }, 'slow');
}

/**
 * (特殊な状況)指定した表示されていないアイテムへのフォーカス移動指示を制御します。<br/>
 * @param itemId 対象アイテムID
 * @returns フォーカスしたかどうか
 */
function focusToItemIfItemInvisible(itemId) {
	if (isNicEdit(itemId)) {
		focusNicEditItem(itemId);
	} else {
		// 表示されていない項目でどう処理すべきか不明なものは、「エラーがあります」へ移動する
    	moveScrollToTargetItem("errorGroup");
		return false;
	}
}

/**** NicEdit関連の特別なメソッド(NicEditを利用しないなら削除可能) ****/
/**
 * 対象アイテム項目がNicEditかどうか判定します。
 * @param itemId 対象アイテムID
 * @returns 対象アイテム項目がNicEditの場合true
 */
function isNicEdit(itemId) {
	var prevTarget = $("#" + itemId).prev();
	if (prevTarget.hasClass("nicEdit-mainContain")) {
		return true;
	} else {
		return false;
	}
}

/**
 * NicEditのアイテムへのフォーカス移動します。<br/>
 * @param itemId 対象アイテムID(NicEdit)
 * @returns フォーカスしたかどうか
 */
function focusNicEditItem(itemId) {
	var prevTarget = $("#" + itemId).prev();
	var itemPosition = prevTarget.offset().top - 25;
	moveScrollToTargetPosition(itemPosition);

	var children = prevTarget.children(".nicEdit-main")[0];
	children.focus();
}
/**** NicEdit関連の特別なメソッド ここまで ****/

/**
 * 対象項目IDを持つエレメントの表示状態を取得します。
 * @param itemId 対象項目ID
 * @returns 表示されている場合true
 */
function isTargetItemVisible(itemId) {
	return $("#" + itemId).is(':visible');
}

/**
 * 対象項目IDを持つエレメントを表示します。
 * @param itemId 対象項目ID
 */
function showArea(itemId) {
	$("#" + itemId).show();
}

/**
 * 対象項目IDを持つエレメントを隠します。
 * @param itemId 対象項目ID
 */
function hideArea(itemId) {
	$("#" + itemId).hide();
}

/**
 * 対象ID(+indexSymbol)のエレメントの値に指定した値をプラスします。(引数がマイナス指定の場合はマイナスされます)
 * @param id 対象エレメントID
 * @param indexSymbol インデックスシンボル(明細の場合指定)
 * @param addCount エレメントに追加される値(数値: マイナス指定可能)
 * @param min 最小設定値
 * @param max 最大設定値
 * @param isSeparate カンマ区切りをするか
 */
function setAddCount(id, indexSymbol, addCount, min, max, isSeparate) {
	var elementId = id + indexSymbol;
	var nowValue = getElementValue(elementId);
	var nowNumberValue;
	try {
		nowNumberValue = Number(removeCanma(nowValue));
	} catch(e) {
		// 数値でない
		return;
	}

	var settingNumberValue = nowNumberValue + addCount;
	if (!isNullorUndefined(min) && settingNumberValue < min) {
		// 最小より小さい
		return;
	}

	if (!isNullorUndefined(max) && max < settingNumberValue) {
		// 最大より大きい
		return;
	}

	if (isSeparate) {
		settingNumberValue = getFormatedNumberString(settingNumberValue, 0, isSeparate)
	}
	setValue(elementId, settingNumberValue, true);

	var settingValue = getElementValue(elementId);
	if (isNullorUndefinedorEmpty(settingValue)) {
		// 元に戻す(コンボなど選択値にないものを設定した可能性を考慮)
		setValue(elementId, nowValue, true);
	}
}

function disableCardInfo() {
	// 通常クレカ
	var cardNo = document.getElementById('creditCardNo8Disp');
	var isCardNoNotMasked = false;
	if(!isNullorUndefinedorEmpty(cardNo)) {
		// 数字だけだったらマスクされてない判定
		isCardNoNotMasked = cardNo.value.match(/^[\d,\.-]+$/);
		if(isCardNoNotMasked) {
			cardNo.disabled = true;
		}
	}
	var enableYaer = document.getElementById('enableYear8');
	if(!isNullorUndefinedorEmpty(enableYaer)) {
		if(isCardNoNotMasked) {
			enableYaer.disabled = true;
		}
	}
	var enableMonth = document.getElementById('enableMonth8');
	if(!isNullorUndefinedorEmpty(enableMonth)) {
		if(isCardNoNotMasked) {
			enableMonth.disabled = true;
		}
	}
	var securityCode = document.getElementById('securityCode8');
	if(!isNullorUndefinedorEmpty(securityCode)) {
		if(isCardNoNotMasked) {
			securityCode.disabled = true;
		}
	}
	var creditCardOwner = document.getElementById('creditCardOwner');
	if(!isNullorUndefinedorEmpty(creditCardOwner)) {
		if(isCardNoNotMasked) {
			creditCardOwner.disabled = true;
		}
	}

	// 店舗受取（オンライン払い）クレカ
	var cardNo29 = document.getElementById('creditCardNo29Disp');
	var isCardNoNotMasked29 = false;
	if(!isNullorUndefinedorEmpty(cardNo29)) {
		// 数字だけだったらマスクされてない判定
		isCardNoNotMasked29 = cardNo29.value.match(/^[\d,\.-]+$/);
		if(isCardNoNotMasked29) {
			cardNo29.disabled = true;
		}
	}
	var enableYaer29 = document.getElementById('enableYear29');
	if(!isNullorUndefinedorEmpty(enableYaer29)) {
		if(isCardNoNotMasked29) {
			enableYaer29.disabled = true;
		}
	}
	var enableMonth29 = document.getElementById('enableMonth29');
	if(!isNullorUndefinedorEmpty(enableMonth29)) {
		if(isCardNoNotMasked29) {
			enableMonth29.disabled = true;
		}
	}
	var securityCode29 = document.getElementById('securityCode29');
	if(!isNullorUndefinedorEmpty(securityCode29)) {
		if(isCardNoNotMasked29) {
			securityCode29.disabled = true;
		}
	}
}

/**
 * 全角・半角を判断し、開始位置～最大文字数までの値を
 * 全角の途中で切れないように返却する
 * @param val 文字
 * @param start 開始位置
 * @param size 最大文字数
 * @returns 開始位置～最大文字
 */
function substrFullHalf(val, start, size) {

	let result = ""
	let countStart = 0
	let countCurrent = 0

	for(let i = 0; i < val.length; i++) {
		const word = val.charAt(i);
		const valSize = String(word).match(/[\x01-\x7E\uFF65-\uFF9F]/) ? 1 : 2

		if (countStart >= start) {
			countCurrent += valSize
			if (countCurrent <= size) {
				result += word;
			} else {
				break;
			}
		}
		countStart += valSize;
	}

	return result
}

/**
 * バイト数の入力制御（半角1Byte、全角2Byteとみなし制御）
 * @param e イベント
 * @param checkLengthByte 最大バイト数
 * @returns 最大バイト数までの入力値
 */
function cutMaxFullHalfCharInput(e, checkLengthByte) {

	let result = substrFullHalf($("#" + e.target.id).val(), 0, checkLengthByte);

	if (result) {
		$("#" + e.target.id).val(result);
	}
}