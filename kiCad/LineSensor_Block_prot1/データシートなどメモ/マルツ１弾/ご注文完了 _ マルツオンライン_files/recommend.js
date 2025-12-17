if (!window._rcmdjp) {
  _rcmdjp = {
    // actions
    _setUser: function(params) {
      _rcmdjp._users.push(params || {});
    },
    _setItemData: function(params, options) {
      params = params || {};

      var inherits = (options || {}).inherit || [];
      if (!(inherits instanceof Array)) inherits = [inherits];
      for (var i = 0, column; column = inherits[i]; i++) params[column] = params[column] || '__inherit__';

      _rcmdjp._itemDataList.push(params);
    },
    _trackRecommend: function(params) {
      params = params || {};
      _rcmdjp._trackedElements.push(_rcmdjp._merge(_rcmdjp._addDefaultRankingParams(params), { mode: params.type || 'pv', template: params.template }));
      _rcmdjp._trackedRecommends.push(_rcmdjp._addDefaultRecommendParams(params));
    },
    _displayRecommend: function(params) {
      params = params || {};
      _rcmdjp._trackRecommend(params);
      _rcmdjp._displayedRecommends.push(_rcmdjp._merge(_rcmdjp._addDefaultRecommendParams(params), { _elementID: params.el || _rcmdjp._writeElement() }));
    },
    _trackRanking: function(params) {
      params = params || {};
      _rcmdjp._trackedElements.push(_rcmdjp._merge(_rcmdjp._addDefaultRankingParams(params), { mode: 'ranking', template: params.template }));
    },
    _displayRanking: function(params) {
      params = params || {};
      _rcmdjp._trackRanking(params);
      _rcmdjp._displayedRankings.push(_rcmdjp._merge(_rcmdjp._addDefaultRankingParams(params), { _elementID: params.el || _rcmdjp._writeElement() }));
    },
    _displayHistory: function(params) {
      params = _rcmdjp._addDefaultHistoryParams(params || {});
      if (_rcmdjp._displayedHistoryNum > 0)
        _rcmdjp._trackedElements.push(_rcmdjp._merge(params, { mode: 'history', template: params.template }));
      _rcmdjp._displayedHistories.push(_rcmdjp._merge(params, { _elementID: params.el || _rcmdjp._writeElement() }));
    },
    _displayMultiDeviceHistory: function(params) {
      params = _rcmdjp._addDefaultHistoryParams(params || {});
      if (_rcmdjp._useMultiDeviceHistory && params.id.length > 0) {
        if (_rcmdjp._displayedHistoryNum > 0)
          _rcmdjp._trackedElements.push(_rcmdjp._merge(params, { mode: 'multi_device_history', template: params.template }));
        _rcmdjp._displayedMultiDeviceHistories.push(_rcmdjp._merge(params, { _elementID: params.el || _rcmdjp._writeElement() }));
      } else {
        if (_rcmdjp._displayedHistoryNum > 0)
          _rcmdjp._trackedElements.push(_rcmdjp._merge(params, { mode: 'history', template: params.template }));
        _rcmdjp._displayedHistories.push(_rcmdjp._merge(params, { _elementID: params.el || _rcmdjp._writeElement() }));
      }
    },
    _displayNewItem: function(params) {
      params = _rcmdjp._addDefaultNewItemParams(params || {});
      _rcmdjp._trackedElements.push(_rcmdjp._merge(params, { mode: 'new_item', template: params.template }));
      _rcmdjp._displayedNewItems.push(_rcmdjp._merge(params, { _elementID: params.el || _rcmdjp._writeElement() }));
    },
    _displayPickup: function(params) {
      params = params || {};
      _rcmdjp._trackedElements.push(_rcmdjp._merge(params, { mode: 'pickup', template: params.template }));
      _rcmdjp._displayedPickups.push(_rcmdjp._merge(params, { _elementID: params.el || _rcmdjp._writeElement() }));
    },
    _trackConversion: function(items, options) {
      items = items || {};
      if (options && options.order) items.__order__ = options.order;
      _rcmdjp._trackedConversions.push(items);
    },
    _trackTourRecommend: function(params) {
      params = params || {};
      _rcmdjp._setItemData(_rcmdjp._addDefaultTourRecommendItemParams(params));
      _rcmdjp._trackRecommend(_rcmdjp._addDefaultRecommendParams(params));
    },
    _displayTourRecommend: function(params) {
      params = params || {};
      _rcmdjp._setItemData(_rcmdjp._addDefaultTourRecommendItemParams(params));
      _rcmdjp._displayRecommend(_rcmdjp._addDefaultRecommendParams(params));
    },
    _disableClickedItem: function() {
      var cookies = _rcmdjp._getCookies('recommend', true);
      if (!cookies || !cookies.clickedURL) return;
      var code = _rcmdjp._getItemCodeForURL(cookies.clickedURL);
      if (!code) return;
      _rcmdjp._setItemData({ code: code, disabled: 1 });
    },
    _displayPersonalizedRecommend: function(params) {
      params = params || {};
      _rcmdjp._trackedElements.push(_rcmdjp._merge(params, { mode: 'personalized_recommend', template: params.template }));
      _rcmdjp._displayedPersonalizedRecommends.push(_rcmdjp._merge(params, { _elementID: params.el || _rcmdjp._writeElement() }));
    },
    _sendTrackBeacons: function(params) {
      try{
        _rcmdjp._trackRecommend(params);
        _rcmdjp._triggerTrackRecommends();
        _rcmdjp._triggerTrackElements();
      } catch(err) {}
    },

    // targets
    _target: {
      _viewed: function() {
        var codes = [];
        var hists = _rcmdjp._getHistory('view');
        var current = _rcmdjp._getItemCodeByURL();
        for (var i = 0; i < hists.length; i++)
          if (!current || current != hists[i]) codes.push(hists[i]);
        return codes.slice(0, _rcmdjp._displayedHistoryNum);
      },
      _converted: function() {
        return _rcmdjp._getHistory('cv');
      }
    },

    // action execution methods
    _trigger: function() {
      _rcmdjp._state = 'process';
      _rcmdjp._observeClick();
      _rcmdjp._triggerSetUsers();
      _rcmdjp._triggerSetItemData();
      _rcmdjp._triggerTrackRecommends();
      _rcmdjp._triggerTrackElements();
      _rcmdjp._triggerTrackConversions();
      _rcmdjp._triggerDisplayRecommends();
      _rcmdjp._triggerDisplayRankings();
      _rcmdjp._triggerDisplayHistories();
      _rcmdjp._triggerDisplayMultiDeviceHistories();
      _rcmdjp._triggerDisplayNewItems();
      _rcmdjp._triggerDisplayPickups();
      _rcmdjp._triggerDisplayPersonalizedRecommends();
      _rcmdjp._state = 'complete';
    },

    _triggerSetUsers: function() {
      for (var i = 0, attr; attr = _rcmdjp._users[i]; i++) {
        if (!attr.id) continue;
        _rcmdjp._ping('user', { userid: attr.id });
      }
    },

    _triggerSetItemData: function() {  // call before _triggerDisplay*
      for (var i = 0, attr; attr = _rcmdjp._itemDataList[i]; i++) {
        attr.code = attr.code || _rcmdjp._getItemCodeByURL();
        if (!attr.code) return;
        attr.name = attr.name || document.title;
        if (!attr.url) attr['auto:url'] = _rcmdjp._location().href;
        var params = { item: attr.code };
        for (var key in attr)
          if (attr.hasOwnProperty(key) && key != 'code')
            params['data:' + _rcmdjp._encodeURIComponent(key.toLowerCase())] = attr[key];
        _rcmdjp._ping('data', params);
        _rcmdjp._itemDataForCode[attr.code] = attr;
      }
      _rcmdjp._itemDataList = [];
    },

    _triggerTrackRecommends: function() {  // call before _triggerTrackElements
      var typesForCode = {};
      for (var i = 0, attr; attr = _rcmdjp._trackedRecommends[i]; i++) {
        if (!attr.code || (attr.code instanceof Array) || (attr.code instanceof Function)) continue;
        var types = typesForCode[attr.code] || [];
        types.push(attr.type);
        typesForCode[attr.code] = types
      }
      var addedParams = {};
      if (_rcmdjp._from().type) {
        addedParams.via = _rcmdjp._from().type;
      }
      if (!_rcmdjp._acceptableReferer()) {
        if (!addedParams.via) addedParams.via = '';
        addedParams.via += 'ignore';
      }
      for (var code in typesForCode) {
        if (!typesForCode.hasOwnProperty(code)) continue;
        _rcmdjp._ping('recommend', _rcmdjp._merge({ type: typesForCode[code].join(','), item: code }, addedParams));
        _rcmdjp._addHistory('view', code);
        _rcmdjp._addMultiDeviceHistory(code);
        if (addedParams.via && addedParams.via != 'ignore') _rcmdjp._addHistory('click', code);
      }
      _rcmdjp._trackedRecommends = [];
    },

    _triggerDisplayRecommends: function() {
      if (_rcmdjp._displayedRecommends.length <= 0) return;
      var recommend = _rcmdjp._displayedRecommends[0];
      if (recommend.code instanceof Function) recommend.code = recommend.code.call();
      if (!recommend.code || !recommend.template) {
        _rcmdjp._displayedRecommends.shift();
        _rcmdjp._triggerDisplayRecommends();
        return;
      }
      if (recommend.code instanceof Array) {
        if (recommend.code.length <= 0) {
          _rcmdjp._renderCombinedRecommend(recommend, []);
          _rcmdjp._displayedRecommends.shift();
          _rcmdjp._triggerDisplayRecommends();
          return;
        }
        recommend._requestCodes = recommend.code.slice(0, Math.min(recommend.code.length, _rcmdjp._combineFetchNum));
        _rcmdjp._itemRecommendCallback = _rcmdjp._combinedRecommendCallback;
        _rcmdjp._jsonp(recommend.type + '_recommend', { type: recommend.type, code: recommend._requestCodes.shift(), filter: recommend.filter });
      } else {
        _rcmdjp._itemRecommendCallback = _rcmdjp._singleRecommendCallback;
        _rcmdjp._jsonp(recommend.type + '_recommend', { type: recommend.type, code: recommend.code, filter: recommend.filter });
      }
    },

    _triggerTrackElements: function() {
      var from = _rcmdjp._from();
      if (!from.type && _rcmdjp._trackedElements.length <= 0) return;

      var modes = [];
      for (var i = 0, attr; attr = _rcmdjp._trackedElements[i]; i++)
        if (attr.mode) modes.push(attr.mode);

      var events = []
      if (from.type && from.code) {
        events.push(['visit', from.code || ''].join(':'));
      }
      if (from.type) {
        events.push(['click', from.type, from.template || ''].join(':'));
      }
      for (var i = 0, attr; attr = _rcmdjp._trackedElements[i]; i++) {
        var mode = attr.mode;
        if (!mode) continue;
        var template = (attr.template instanceof Array) ? _rcmdjp._selectTemplate(attr.template) : attr.template;
        events.push(['view', mode, template || ''].join(':'));
      }

      _rcmdjp._ping('track', { mode: modes.join(','), event: events.join(',') });
      _rcmdjp._trackedElements = [];
    },

    _triggerDisplayRankings: function() {
      var sendKeys = {};
      for (var i = 0, attr; attr = _rcmdjp._displayedRankings[i]; i++) {
        if (!attr.template) continue;

        var category_column = false;
        for (var key in attr) {
          if (key.match(/^category.+$/)) {
            category_column = key;
            break;
          }
        }

        var key = [category_column || '', attr.category, attr.type, attr.span].join('/');
        if (key in sendKeys) continue;

        if (category_column) {
          attr._multi = true;
          _rcmdjp._jsonp('multi_category_ranking', { column: category_column, category: attr[category_column], type: attr.type, span: attr.span  });
        } else if (attr.category == 'default')
          _rcmdjp._jsonp('ranking', { type: attr.type, span: attr.span  });
        else
          _rcmdjp._jsonp('category_ranking', { category: attr.category, type: attr.type, span: attr.span  });
      }
    },

    _triggerDisplayHistories: function() {
      if (_rcmdjp._displayedHistories.length <= 0) return;
      if (_rcmdjp._displayedHistoryNum <= 0) {
        _rcmdjp._renderHistories([]);
        return;
      }
      var codes = _rcmdjp._getHistory('view');
      for (var i = 0, attr; attr = _rcmdjp._displayedHistories[i]; i++) {
        var appeared = {};
        if (attr.code) {
          if (attr.code instanceof Array) {
            for (var k = 0, e; e = attr.code[k]; k++) appeared[e] = true;
          } else {
            appeared[attr.code] = true;
            _rcmdjp._addHistory('view', attr.code);
          }
          var cs = [];
          for (var j = 0, c; c = codes[j]; j++) if (!appeared[c]) cs.push(c);
          codes = cs;
        }
      }
      if (codes.length <= 0) {
        _rcmdjp._renderHistories([]);
      } else {
        _rcmdjp._historyRequests = codes.slice(0, _rcmdjp._displayedHistoryNum*2);
        _rcmdjp._historyRequestRests = codes.slice(_rcmdjp._displayedHistoryNum*2);
        _rcmdjp._historyItems = [];
        _rcmdjp._historyLoadedItemNum = 0;
        for (var k = 0, cd; cd = _rcmdjp._historyRequests[k]; k++) _rcmdjp._jsonp('item', { code: cd });
      }
    },

    _triggerDisplayMultiDeviceHistories: function() {
      if (_rcmdjp._displayedMultiDeviceHistories.length <= 0) return;
      if (_rcmdjp._displayedHistoryNum <= 0) {
        _rcmdjp._renderMultiDeviceHistories([]);
        return;
      }
      _rcmdjp._getMultiDeviceHistory();
    },

    _triggerDisplayNewItems: function() {
      for (var i = 0, attr; attr = _rcmdjp._displayedNewItems[i]; i++) {
        if (!attr.template) continue;
        var category_column = false;
        for (var key in attr) {
          if (key.match(/^category.+$/)) {
            category_column = key;
            break;
          }
        }
        if (category_column) {
          attr._multi = true;
          _rcmdjp._jsonp('multi_category_new_item', { column: category_column, category: attr[category_column], span: attr.span  });
        } else if (attr.category == 'default') {
          _rcmdjp._jsonp('new_item', { span: attr.span  });
        } else {
          _rcmdjp._jsonp('category_new_item', { category: attr.category, span: attr.span  });
        }
      }
    },

    _triggerDisplayPickups: function() {
      for (var i = 0, attr; attr = _rcmdjp._displayedPickups[i]; i++) {
        if (!attr.label || !attr.template) continue;
        _rcmdjp._jsonp('pickup', { label: attr.label });
      }
    },

    _triggerTrackConversions: function() {
      if (_rcmdjp._trackedConversions.length <= 0) return;
      var params = {};
      var clickedCodes = _rcmdjp._getHistory('click');
      for (var i = 0, cv; cv = _rcmdjp._trackedConversions[i]; i++) {
        if (cv.__order__) params.order = cv.__order__;
        for (var key in cv) {
          if (!cv.hasOwnProperty(key) || key == '__order__') continue;
          _rcmdjp._addHistory('cv', key);
          var via = false;
          for (var j = 0, clicked; clicked = clickedCodes[j]; j++) {
            if (key == clicked) {
              via = true;
              break;
            }
          }
          params['cv:' + _rcmdjp._encodeURIComponent(key)] = String(cv[key]) + '/' + (via ? '1' : '0');
        }
      }
      _rcmdjp._ping('conversion', params);
    },

    _triggerDisplayPersonalizedRecommends: function() {
      if (_rcmdjp._displayedPersonalizedRecommends.length <= 0) return;
      var prcmd = _rcmdjp._displayedPersonalizedRecommends[0];
      if (!prcmd.type || !prcmd.user || !prcmd.template) {
        _rcmdjp._displayedPersonalizedRecommends.shift();
        _rcmdjp._triggerDisplayPersonalizedRecommends();
        return;
      }
      _rcmdjp._jsonp('personalized_recommend', { type: prcmd.type, user: prcmd.user });
    },

    _observeClick: function() {
      if (_rcmdjp._displayedRecommends.length > 0 || _rcmdjp._trackedRecommends <= 0) return;
      var recommend = _rcmdjp._trackedRecommends[0];
      _rcmdjp._observeEvent(document, 'click', function(event) {
        var viaRegex = /[\?\&]rcmdjp(=[^\?\&]*)?/;
        var element = window.event ? window.event.srcElement : event.target;
        if (element && element.nodeType == 3) element = element.parentNode;
        while (element && element.nodeType == 1) {
          if (element.tagName.toUpperCase() == 'A') {
            if ((element.className && element.className.match(/rcmdjp/)) || (element.href && element.href.match(viaRegex))) {
              _rcmdjp._setCookies({ from: 'recommend' }, 60, 'recommend');
              _rcmdjp._trackEvent('click', 'recommend');
            }
            return;
          }
          element = element.parentNode;
        }
      });
    },

    // jsonp callbacks
    _singleRecommendCallback: function(code, filter, items) {
      if (_rcmdjp._displayedRecommends.length <= 0) return;
      _rcmdjp._renderRecommend(_rcmdjp._displayedRecommends.shift(), items);
      _rcmdjp._triggerDisplayRecommends();
    },
    _combinedRecommendCallback: function(code, filter, items) {
      if (_rcmdjp._displayedRecommends.length <= 0) return;
      var recommend = _rcmdjp._displayedRecommends[0];
      recommend._items = recommend._items || [];
      if (items.length > 0) recommend._items.push(items);
      if (recommend._requestCodes.length > 0) {
        _rcmdjp._jsonp(recommend.type + '_recommend', { type: recommend.type, code: recommend._requestCodes.shift(), filter: recommend.filter });
      } else {
        _rcmdjp._renderCombinedRecommend(recommend, recommend._items);
        _rcmdjp._displayedRecommends.shift();
        _rcmdjp._triggerDisplayRecommends();
      }
    },
    _renderCombinedRecommend: function(recommend, itemsList) {
      var items = [];
      var appeared = {};
      for (var i = 0, code; code = recommend.code[i]; i++) appeared[code] = true;
      loop:
        while (itemsList.length > 0) {
          var list = [];
          for (var i = 0, is; is = itemsList[i]; i++) {
            if (items.length >= _rcmdjp._displayedRecommendNum) break loop;
            var item = is.shift();
            if (item) {
              if (!appeared[item.code]) {
                items.push(item);
                appeared[item.code] = true;
              }
              list.push(is);
            }
          }
          itemsList = list;
        }
      _rcmdjp._renderRecommend(recommend, items);
    },
    _renderRecommend: function(recommend, items) {
      _items = _rcmdjp._filteringItem(items, _rcmdjp._displayedRecommendNum);
      _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(recommend._elementID), recommend.template, _items, 'recommend', recommend.code, recommend.el_append);
    },
    _rankingCallback: function(type, span, items) {
      _rcmdjp._categoryRankingCallback('default', type, span, items);
    },
    _categoryRankingCallback: function(category, type, span, items) {
      items = _rcmdjp._filteringItem(items, _rcmdjp._displayedRankingNum);
      var rankings = [];
      var rest_rankings = [];
      for (var i = 0, attr; attr = _rcmdjp._displayedRankings[i]; i++) {
        ((attr.category == category && attr.type == type && attr.span == span && !attr._multi) ? rankings : rest_rankings).push(attr);
      }
      _rcmdjp._displayedRankings = rest_rankings;
      for (var j = 0, ranking; ranking = rankings[j]; j++) {
        _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(ranking._elementID), ranking.template, items, 'ranking', false, ranking.el_append);
      }
    },
    _multiCategoryRankingCallback: function(column, category, type, span, items) {
      items = _rcmdjp._filteringItem(items, _rcmdjp._displayedRankingNum);
      var rankings = [];
      var rest_rankings = [];
      for (var i = 0, attr; attr = _rcmdjp._displayedRankings[i]; i++) {
        (((column in attr) && attr[column] == category && attr.type == type && attr.span == span) ? rankings : rest_rankings).push(attr);
      }
      _rcmdjp._displayedRankings = rest_rankings;
      for (var j = 0, ranking; ranking = rankings[j]; j++) {
        _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(ranking._elementID), ranking.template, items, 'ranking', false, ranking.el_append);
      }
    },
    _itemCallback: function(item) {
      if (item.code == '__alt__' || !item.name || item.disabled) {
        var code = _rcmdjp._historyRequestRests.shift();
        if (code) {
          _rcmdjp._historyRequests.push(code);
          _rcmdjp._jsonp('item', { code: code });
        }
      } else {
        for (var i = 0; i < _rcmdjp._historyRequests.length; i++) {
          if (_rcmdjp._historyRequests[i] == item.code) {
            _rcmdjp._historyItems[i] = item;
            break;
          }
        }
      }
      if (++_rcmdjp._historyLoadedItemNum >= _rcmdjp._historyRequests.length) {
        var items = [];
        for (var j = 0; j < _rcmdjp._historyItems.length; j++) {
          if (_rcmdjp._historyItems[j]) items.push(_rcmdjp._historyItems[j]);
        }
        items = _rcmdjp._filteringItem(items, _rcmdjp._displayedHistoryNum);
        _rcmdjp._renderHistories(items);
      }
    },

    _renderHistories: function(items) {
      for (var i = 0, hist; hist = _rcmdjp._displayedHistories[i]; i++) {
        _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(hist._elementID), hist.template, items, 'history', false, hist.el_append);
      }
    },

    _renderMultiDeviceHistories: function(items) {
      for (var i = 0, hist; hist = _rcmdjp._displayedMultiDeviceHistories[i]; i++) {
        _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(hist._elementID), hist.template, items, 'multi_device_history', false, hist.el_append);
      }
    },
    _newItemCallback: function(span, items) {
      items = _rcmdjp._filteringItem(items, _rcmdjp._displayedNewItemNum);
      var targets = [], rest = [];
      for (var i = 0, attr; attr = _rcmdjp._displayedNewItems[i]; i++) {
        ((attr.category == 'default' && attr.span == span && !attr._multi) ? targets : rest).push(attr);
      }
      _rcmdjp._displayedNewItems = rest;
      for (var j = 0, target; target = targets[j]; j++) {
        _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(target._elementID), target.template, items, 'new_item', false, target.el_append);
      }
    },
    _categoryNewItemCallback: function(category, span, items) {
      items = _rcmdjp._filteringItem(items, _rcmdjp._displayedNewItemNum);
      var targets = [], rest = [];
      for (var i = 0, attr; attr = _rcmdjp._displayedNewItems[i]; i++) {
        ((attr.category == category && attr.span == span && !attr._multi) ? targets : rest).push(attr);
      }
      _rcmdjp._displayedNewItems = rest;
      for (var j = 0, target; target = targets[j]; j++) {
        _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(target._elementID), target.template, items, 'new_item', false, target.el_append);
      }
    },
    _multiCategoryNewItemCallback: function(column, category, span, items) {
      items = _rcmdjp._filteringItem(items, _rcmdjp._displayedNewItemNum);
      var targets = [], rest = [];
      for (var i = 0, attr; attr = _rcmdjp._displayedNewItems[i]; i++) {
        (((column in attr) && attr[column] == category && attr.span == span) ? targets : rest).push(attr);
      }
      _rcmdjp._displayedNewItems = rest;
      for (var j = 0, target; target = targets[j]; j++) {
        _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(target._elementID), target.template, items, 'new_item', false, target.el_append);
      }
    },
    _pickupCallback: function(label, items) {
      items = _rcmdjp._filteringItem(items, _rcmdjp._displayedPickupNum);
      var targets = [], rest = [];
      for (var i = 0, attr; attr = _rcmdjp._displayedPickups[i]; i++) {
        (attr.label == label ? targets : rest).push(attr);
      }
      _rcmdjp._displayedPickups = rest;
      for (var j = 0, target; target = targets[j]; j++) {
        _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(target._elementID), target.template, items, 'pickup', false, target.el_append);
      }
    },
    _personalizedRecommendCallback: function(type, user, items) {
      items = _rcmdjp._filteringItem(items, _rcmdjp._displayedPersonalizedNum);
      if (_rcmdjp._displayedPersonalizedRecommends.length <= 0) return;
      var personalizedRcmd = _rcmdjp._displayedPersonalizedRecommends.shift();
      _rcmdjp._renderTemplate(_rcmdjp._adaptElementID(personalizedRcmd._elementID), personalizedRcmd.template, items, 'personalized_recommend', false, personalizedRcmd.el_append);
      _rcmdjp._triggerDisplayPersonalizedRecommends();
    },

    // global variables
    _users: [],
    _itemDataList: [],
    _itemDataForCode: {},
    _trackedRecommends: [],
    _trackedElements: [],
    _trackedConversions: [],
    _displayedRecommends: [],
    _displayedRankings: [],
    _displayedHistories: [],
    _displayedMultiDeviceHistories: [],
    _displayedNewItems: [],
    _displayedPickups: [],
    _displayedPersonalizedRecommends: [],
    _historyRequests: [],
    _historyItems: [],
    _historyLoadedItemNum: 0,
    _elementIndex: 0,
    _pingedURLs: [],
    _state: 'wait',
    _observedEvents: {},

    // internal methods
    _location: function() {
      return location;
    },
    _addDefaultRankingParams: function(params) {
      return _rcmdjp._merge(params, {
        type: params.type || 'pv',
        span: params.span || 'day',
        category: params.category || 'default'
      })
    },
    _addDefaultRecommendParams: function(params) {
      return _rcmdjp._merge(params, {
        code: params.code || _rcmdjp._getItemCodeByURL(),
        type: params.type || 'pv',
        filter: params.filter || 'default'
      });
    },
    _addDefaultHistoryParams: function(params) {
      return _rcmdjp._merge(params, {
        code: params.code || _rcmdjp._getItemCodeByURL()
      });
    },
    _addDefaultNewItemParams: function(params) {
      return _rcmdjp._merge(params, {
        span: params.span || 'default',
        category: params.category || 'default'
      })
    },
    _addDefaultTourRecommendItemParams: function(params) {
      attr = { name: params.name || document.title };
      attr.code = attr.url = params.code || _rcmdjp._getItemCodeByURL();
      if (attr.url.charAt(0) == '/')
        attr.url = _rcmdjp._location().protocol + '//' + _rcmdjp._location().host + attr.url;
      if (params.titleRegex) {
        var match = attr.name.match(params.titleRegex);
        if (match && match[1]) attr.name = match[1];
      }
      if (params.titlePrefix)
        if (attr.name.indexOf(params.titlePrefix) == 0)
          attr.name = attr.name.slice(params.titlePrefix.length);
      if (params.titleSuffix)
        if (attr.name.indexOf(params.titleSuffix) == attr.name.length - params.titleSuffix.length)
          attr.name = attr.name.slice(0, - params.titleSuffix.length);
      if (params.data)
        for (var key in params.data)
          if (params.data.hasOwnProperty(key)) attr[key] = params.data[key];
      return attr;
    },

    _writeElement: function() {
      var id = '_rcmdjp_display_' + String(++_rcmdjp._elementIndex);
      const elm = document.createElement('div');
      elm.id = id;
      document.currentScript.after(elm);
      return id;
    },
    _getItemCodeByURL: function() {
      if (location.protocol == 'file:') return null;
      try { return eval(_rcmdjp._itemCodeGetter); } catch (e) {}
      return null;
    },
    _getItemCodeForURL: function(url) {
      var location = _rcmdjp._parseURL(url);
      try { return eval(_rcmdjp._itemCodeGetter); } catch (e) {}
      return null;
    },
    _parseURL: function(url) {
      var m = url.match(/^(\w+:)\/\/(([^:\/]+)(:(\d+))?)(\/[^\?#]*)(\?[^#]*)?(#.*)?$/);
      if (!m) return null;
      return { href: url, protocol: m[1] || '', host: m[2] || '', hostname: m[3] || '', port: m[5] || '', pathname: m[6] || '', search: m[7] || '', hash: m[8] || '' };
    },
    _ping: function(type, params) {
      params.account = _rcmdjp._accountName;
      params.recommend = _rcmdjp._recommendName;
      params.uid = _rcmdjp._getUID();
      params.timestamp = (new Date()).getTime();
      var img = new Image(1, 1);
      img.src = _rcmdjp._sslize(_rcmdjp._pingURLs[type]) + '?' + _rcmdjp._toQueryString(params);
      img.onload = function() {};
      _rcmdjp._pingedURLs.push(img.src);
    },
    _beacon: function(url, params) {
      params || (params = {});
      var img = new Image(1, 1);
      img.src = url + '?' + _rcmdjp._toQueryString(params);
      img.onload = function() {};
    },
    _jsonp: function(type, params) {
      var url = _rcmdjp._sslize(_rcmdjp._jsonpURLs[type]);
      for (var key in params)
        if (params.hasOwnProperty(key))
          url = url.replace('#{' + key + '}', _rcmdjp._encodeURIComponent2(params[key]));
      var s = document.createElement('script');
      s.type = 'text/javascript';
      s.charset = 'UTF-8';
      s.src = url + '?' + new Date().getTime();
      document.body.appendChild(s);
    },
    _trackEvent: function(type, label) {
      if (window.ga) {
        window.ga('send', 'event', 'rcmdjp', type, label);
      }
      if (window._gaq && window._gaq.push) {
        window._gaq.push(['_trackEvent', 'rcmdjp', type, label]);
      }
      if (window.pageTracker && window.pageTracker._trackEvent) {
        window.pageTracker._trackEvent('rcmdjp', type, label);
      }
      if (window.gtag) {
        window.gtag('event', type, {
          'event_category': 'rcmdjp',
          'event_label': label
        });
      }
      if ('dataLayer' in window) {
        dataLayer.push({'event_category': 'rcmdjp', 'event_label': label})
      }
    },
    _renderTemplate: function(element, template_name, items, eventLabel, targetCode, elAppend) {
      if (!element) return false;
      if (template_name instanceof Array) template_name = _rcmdjp._selectTemplate(template_name);
      var template = _rcmdjp._templates[template_name];
      if (!template) return false;
      for (var i = 0, item; item = items[i]; i++) item['url'] += ';_rcmdjp';
      var targetItem = null;
      if (targetCode && (targetCode in _rcmdjp._itemDataForCode)) targetItem = _rcmdjp._itemDataForCode[targetCode];
      var renderer = new _rcmdjp._Renderer();

      var content;
      _rcmdjp._callItemRender(function() {
        content = renderer._render(targetItem, items, template[0]);
      }, function() {
        var dom_content = document.createRange().createContextualFragment(content);
        switch (elAppend) {
          case 'top':
            element.prepend(dom_content);
            break;
          case 'bottom':
            element.append(dom_content);
            break;
          case 'before':
            element.before(dom_content);
            break;
          case 'after':
            element.after(dom_content);
            break;
          default:
            // for table in IE, remove from DOM tempolary
            var swap = document.createElement('div');
            element.parentNode.replaceChild(swap, element);
            element.innerHTML = content;
            swap.parentNode.replaceChild(element, swap);
        }
      }, function() {
        _rcmdjp._addCSSText(template[1]);
      }, function() {
        if(elAppend == 'before' || elAppend == 'after' ) {
          var links = element.parentNode.getElementsByTagName('a');
        }else{
          var links = element.getElementsByTagName('a');
        }
        var linkMarkerRegex = new RegExp(/;_rcmdjp/);
        for (var j = 0, link; link = links[j]; j++) {
          if (!link.href) continue;
          var match = link.href.match(linkMarkerRegex);
          if (match) {
            var html = link.innerHTML;
            link.href = link.href.replace(linkMarkerRegex, '');
            link.innerHTML = html;
          }
          if (match || (link.className && link.className.match(/rcmdjp/))) {
            (function() {
              var url = link.href;
              var clickedCode = '';
              for (var i = 0, item; item = items[i]; i++) {
                var iUrl = _rcmdjp._filteringDomain(item['url']);
                var lUrl = _rcmdjp._filteringDomain(url + ';_rcmdjp');
                if (iUrl == lUrl) clickedCode = item['code'];
              }
              _rcmdjp._observeEvent(link, 'click', function() {
                _rcmdjp._setCookies({ from: eventLabel, template: template_name, clickedURL: url, code: clickedCode }, 60, 'recommend');
                _rcmdjp._trackEvent('click', template_name);
              });
            })();
          }
        }
        for (i = 0, item; item = items[i]; i++) if (item.url) item.url = item.url.replace(linkMarkerRegex, '');
      }, function() {
        renderer._evalScripts();
      });

      return true;
    },
    _selectTemplate: function(templates) {
      var cookies = _rcmdjp._getCookies('template');
      if (!cookies || !cookies.seed) {
        cookies = { seed: Math.floor(Math.random() * 10000) + 1 };
        _rcmdjp._setCookies(cookies, 60 * 60 * 24 * 7, 'template');
      }
      return templates[parseInt(cookies.seed) % templates.length];
    },
    _addCSSText: function(text) {
      var s = document.createElement('style');
      s.setAttribute('type', 'text/css');
      if (s.styleSheet)
        s.styleSheet.cssText = text;
      else
        s.appendChild(document.createTextNode(text));
      document.getElementsByTagName('head')[0].appendChild(s);
    },

    _getHistory: function(type) {
      return merge(cookie(type), storage(type));

      function cookie(type) {
        var cookie = _rcmdjp._getCookies('history')[type] || '';
        return (cookie) ? cookie.split(',') : [];
      }
      function storage(type) {
        if (!_rcmdjp._canUseLocalStorage()) return [];
        var storage = localStorage.getItem('_rcmdjp_history_' + type) || '';
        return (storage) ? storage.split(',') : [];
      }
      function merge(cookie, storage) {
        if (!storage.length) return cookie;
        var add = [], exists = {};
        for (var i = 0, l = cookie.length; i < l; i++) {
          exists[cookie[i]] = true;
        }
        for (var i = 0, l = storage.length; i < l; i++) {
          if (!exists[storage[i]]) add.push(storage[i])
        }
        return cookie.concat(add);
      }
    },

    _getMultiDeviceHistory: function() {
      if (!_rcmdjp._isMultiDeviceHistoryUser()) return _rcmdjp._renderMultiDeviceHistories([]);
      _rcmdjp._loadJquery(function($) {
        var load_params = "?account=" + _rcmdjp._accountName +
          "&recommend=" + _rcmdjp._recommendName +
          "&userid=" + _rcmdjp._getUserId() +
          "&displaynum=" + _rcmdjp._displayedHistoryNum +
          "&code=" + _rcmdjp._displayedMultiDeviceHistories[0].code;
        $.ajax({
          url: _rcmdjp._multiDeviceHistoryURLs['get'] + load_params,
          dataType: 'json',
          method: 'GET',
          cache: false,
          timeout: 1000 * 3,
          success: function(data) {
            _rcmdjp._users = [];
            _rcmdjp._renderMultiDeviceHistories(data.slice(0, _rcmdjp._displayedHistoryNum));
          },
          error : function(){
            _rcmdjp._renderMultiDeviceHistories([]);
          }
        })
      });
    },

    _addHistory: function(type, code) {
      if (!code) return;
      var codes = [],
        oldCodes = _rcmdjp._getHistory(type),
        byte = _rcmdjp._savedHistoryByte[type];
      codes = add(oldCodes, code);
      codes = truncate(codes, byte);
      setCookie(codes, type);
      afterSetCookie(type);

      function add(oldCodes, code) {
        var codes = [code];
        for (var i = 0, c; c = oldCodes[i]; i++) {
          if (c != code) codes.push(c);
        }
        return codes;
      }
      function truncate(codes, byte) {
        var cookie = codes.join(',');
        while (_rcmdjp._encodeURIComponent(cookie).length > byte) {
          cookie = cookie.substr(0, cookie.lastIndexOf(','));
        }
        return cookie.split(',');
      }
      function setCookie(codes, type) {
        var cookies = {}, cookie = codes.join(',');
        cookies[type] = cookie;
        _rcmdjp._setCookies(cookies, 60 * 60 * 24 * 360, 'history');
      }
      function afterSetCookie(type) {
        if (_rcmdjp._canUseLocalStorage()) localStorage.removeItem('_rcmdjp_history_' + type);
        if (_rcmdjp._invalidDomain != '') {
          document.cookie = '_rcmdjp_history_'+type+'=;domain='+_rcmdjp._invalidDomain+';path=/;expires=Thu, 01 Jan 1970 00:00:01 GMT;';
        }
      }
    },

    _addMultiDeviceHistory: function(code) {
      if (!code) return;
      if (!_rcmdjp._isMultiDeviceHistoryUser()) return;
      var data = {
        code: code,
        userid: _rcmdjp._getUserId(),
        account: _rcmdjp._accountName,
        recommend: _rcmdjp._recommendName,
        uid: _rcmdjp._getUID()
      }

      _rcmdjp._loadJquery(function($) {
        $.ajax({
          type: "POST",
          url: _rcmdjp._multiDeviceHistoryURLs['post'],
          contentType: "application/json; charset=utf-8",
          data: JSON.stringify(data),
          dataType: "json",
          processData: false,
          timeout: 1000 * 3
        });
      });
    },

    _filteringItem: function(items, display_num) {
      function checkDisplay(item) {
        var now = new Date();
        if (item.display_start) {
          var display_start = new Date(item.display_start.replace(/-/g,"/"));
          if (now < display_start)
            return false
        }
        if (item.display_end) {
          var display_end = new Date(item.display_end.replace(/-/g,"/"));
          if (now > display_end)
            return false
        }
        return true
      }

      items = items.filter(checkDisplay)
      items = items.slice(0,display_num);
      return items;
    },

    _getUserId: function() {
      return _rcmdjp._users[0].id;
    },

    _isMultiDeviceHistoryUser: function() {
      if (!_rcmdjp._useMultiDeviceHistory) return false;
      if (!_rcmdjp._users.length > 0 || !_rcmdjp._users[0].id) return false;
      return true;
    },

    _getUID: function() {
      var user = _rcmdjp._getCookies('user');
      if (!user.id) {
        user = { id: _rcmdjp._generateUID() };
      }
      _rcmdjp._setCookies(user, 60 * 60 * 24 * 360, 'user');
      if (_rcmdjp._invalidDomain != '') {
        document.cookie = '_rcmdjp_user_id=;domain='+_rcmdjp._invalidDomain+';path=/;expires=Thu, 01 Jan 1970 00:00:01 GMT;';
      }
      return user.id || '';
    },
    _generateUID: function() {
      return _rcmdjp._getCookieDomain() + '-' + String(Math.floor(Math.random() * 2147483647));
    },

    _acceptableReferer: function() {
      var ref = document.referrer;
      if (!ref) return true;
      for (var i = 0, r; r = _rcmdjp._ignoreReferes[i]; i++)
        if (ref.match(r)) return false;
      return true;
    },

    // utility methods
    _merge: function() {
      var results = {};
      for (var i = 0; i < arguments.length; i++) {
        var hash = arguments[i];
        for (var key in hash)
          if (hash.hasOwnProperty(key)) results[key] = hash[key];
      }
      return results;
    },
    _encodeURIComponent: function(str) {
      if (str !== 0 && !str) return '';
      return String(str).replace(/[^!'-*.0-9A-Z_a-z~-]/g, function(s){
        var c=s.charCodeAt(0);
        return (c<16?'%0'+c.toString(16):c<128?'%'+c.toString(16):c<2048?'%'+(c>>6|192).toString(16)+'%'+(c&63|128).toString(16):'%'+(c>>12|224).toString(16)+'%'+(c>>6&63|128).toString(16)+'%'+(c&63|128).toString(16)).toLowerCase();
      })
    },
    _encodeURIComponent2: function(str) {
      return _rcmdjp._encodeURIComponent(_rcmdjp._encodeURIComponent(str));
    },
    _toQueryString: function(hash) {
      var queries = [];
      for (var key in hash)
        if (hash.hasOwnProperty(key)) queries.push(key + '=' + _rcmdjp._encodeURIComponent(hash[key]));
      return queries.join('&');
    },
    _sslize: function(url) {
      return _rcmdjp._location().protocol == 'https:' ?  url.replace(/^http:/, 'https:') : url;
    },
    _filteringDomain: function(url) {
      return (url.match(/^https?:\/\/[^\/]*/)) ? url.replace(/https?:\/\/[^\/]*/,'') : url;
    },
    _observeEvent: function(element, name, handler) {
      if (element.addEventListener) {
        element.addEventListener(name, handler, false);
      } else {
        element.attachEvent("on" + name, handler);
      }
      saveEvent(element, name, handler);

      function saveEvent(element, name, handler) {
        var eventKey = _rcmdjp._getData(element, 'event');
        if (!eventKey) {
          eventKey = createEventKey();
          _rcmdjp._setData(element, 'event', eventKey);
          _rcmdjp._observedEvents[eventKey] = [];
        }
        _rcmdjp._observedEvents[eventKey].push({type: name, handler: handler});
      }
      function createEventKey() {
        return '_rcmdjp_event_' + (countObservedEvents() + 1);
      }
      function countObservedEvents() {
        var i = 0;
        for (var p in _rcmdjp._observedEvents) {
          if (_rcmdjp._observedEvents.hasOwnProperty(p)) i++;
        }
        return i;
      }
    },
    _setData: function (element, key, value) {
      if (element.dataset) {
        element.dataset['rcmdjp' + _rcmdjp._pascalCase(key)] = value;
      } else if (element.setAttribute) {
        element.setAttribute('data-rcmdjp-' + _rcmdjp._dashedCase(key), value);
      }
    },
    _getData: function (element, key) {
      if (element.dataset) {
        return element.dataset['rcmdjp' + _rcmdjp._pascalCase(key)];
      }
      if (element.getAttribute) {
        return element.getAttribute('data-rcmdjp-' + _rcmdjp._dashedCase(key));
      }
      return false;
    },
    _pascalCase: function(str) {
      return _rcmdjp._camelCase(str).replace(/^./, str.charAt(0).toUpperCase());
    },
    _camelCase: function(str) {
      return str.replace(/[^\-_][\-_][^\-_]/g, function (s) {
        return s.charAt(0) + s.charAt(2).toUpperCase();
      });
    },
    _snakeCase: function(str) {
      str = str.replace(/^./, str.charAt(0).toLowerCase());
      str = str.replace(/([A-Z])/g, '_$1').toLowerCase();
      return str;
    },
    _dashedCase: function (str) {
      str = str.replace(/^./, str.charAt(0).toLowerCase());
      str = str.replace(/([A-Z])/g, '-$1').toLowerCase();
      return str;
    },
    _observeLoad: function(callback) {
      var callbackComplate = false;
      if(document.readyState == 'complete'){
        if(!callbackComplate) {
          callbackComplate = true;
          setTimeout(callback, 0);
        }
      }else{
        document.addEventListener('readystatechange', function() {
          if(document.readyState == 'interactive' || document.readyState == 'complete') {
            if(!callbackComplate) {
              callbackComplate = true;
              callback();
            }
          }
        });
      }
    },
    _setCookies: function(values, expireSeconds, prefix) {
      var expires = new Date();
      expires.setTime(expires.getTime() + expireSeconds * 1000);
      for (var key in values)
        if (values.hasOwnProperty(key)) {
          document.cookie = '_rcmdjp_' + prefix + '_' + key + '=' + _rcmdjp._encodeURIComponent(values[key]) + ';domain=' + _rcmdjp._getCookieDomain() + ';path=/;expires=' + expires.toGMTString();
          _rcmdjp._setLocalStorage('_rcmd_jp_' + prefix + '_' + key, _rcmdjp._encodeURIComponent(values[key]), {maxAge: expires});

          // for safari applied ITP2.3
          if (_rcmdjp._cname) {
            var domain = _rcmdjp._cookieDomain || document.domain;
            var secure = (_rcmdjp._allSecure ? 'on' : 'off');
            var url = 'https://' + _rcmdjp._cname + '/ssc/' + secure;
            url += ';name=' + '_rcmdjp_' + prefix + '_' + key;
            url += ';domain=' + domain;
            _rcmdjp._beacon(url, {t: (new Date()).getTime()});
          }
        }
    },
    _setLocalStorage: function(key, value, options) {
      try {
        if (window.localStorage && options.maxAge) {
          var item = {
            value: value,
            expires: options.maxAge
          };
          localStorage.setItem(key, JSON.stringify(item))
        }
      } catch(e) {}
    },
    _getCookieDomain: function() {
      return _rcmdjp._cookieDomain || _rcmdjp._location().hostname;
    },
    _getCookies: function(prefix, remove) {
      var cookies = document.cookie.split(/;\s*/);
      var values = {};
      var fullPrefix = '_rcmdjp_' + prefix + '_';
      for (var i = 0, str; str = cookies[i]; i++) {
        var pair = str.split('=');
        if (pair.length < 2 || pair[0].indexOf(fullPrefix) != 0) continue;
        values[pair[0].slice(fullPrefix.length)] = decodeURIComponent(pair[1]);
      }
      if (JSON.stringify(values) == "{}") values = _rcmdjp._getLocalStorage(prefix);
      if (remove) _rcmdjp._deleteCookies(values, prefix);
      return values;
    },
    _getLocalStorage: function(prefix) {
      function validateExpires(time) {
        var current = (new Date()).getTime();
        var maxTime = current + (60 * 60 * 24 * 360 * 1000);
        time = Date.parse(time);
        if (time > maxTime) {
          return false;
        }
        return time >= current;
      }
      var values = {};
      try {
        if (window.localStorage) {
          var fullPrefix = '_rcmd_jp_' + prefix + '_';
          for(var i=0;i<localStorage.length;i++){
            var key = localStorage.key(i);
            if (key.indexOf(fullPrefix) != 0) continue;
            var item = localStorage.getItem(key);
            var data = JSON.parse(item);
            if (!validateExpires(data.expires)) continue;
            if (item) values[key.slice(fullPrefix.length)] = decodeURIComponent(data.value);
          }
        }
        return values;
      } catch(e) {
        return values;
      }
    },
    _deleteCookies: function(values, prefix) {
      var cookies = {};
      for (var key in values) cookies[key] = '';
      _rcmdjp._setCookies(cookies, -1000, prefix);
    },

    _canUseLocalStorage: function() {
      try{
        return !!window.localStorage;
      } catch(e) {
        return false;
      }
    },
    _useCookieStorage: function() {
      _rcmdjp._canUseLocalStorage = function() { return false; };
    },

    _from: function() {
      if (!_rcmdjp._currentFrom) {
        var from = {};
        var cookies = _rcmdjp._getCookies('recommend', true);
        if ((_rcmdjp._location().search || '').match(/\brcmd_via=mail\b/)) {
          from.type = 'mail';
          mail_type = (_rcmdjp._location().search || '').match(/\brcmd_via=mail:(\w+)\b/);
          if (mail_type) from.template = mail_type[1];
          code = (_rcmdjp._location().search || '').match(/\bvisit=([^&]*)\b/);
          if (code) from.code = code[1];
        } else if ((_rcmdjp._location().search || '').match(/\brcmd_via=api\b/)) {
          from.type = 'api';
          code = (_rcmdjp._location().search || '').match(/\bvisit=([^&]*)\b/);
          if (code) from.code = code[1];
        } else if (cookies.from) {
          from.type = cookies.from;
          from.template = cookies.template;
          from.code = cookies.code;
        }
        _rcmdjp._currentFrom = from;
      }
      return _rcmdjp._currentFrom;
    },

    _callItemRender: function() {
      var fns = arguments;
      if (!fns.length) return;
      for (var i = 0, fn; fn = fns[i]; i++) fn();
    },

    _clone: function(obj) {
      if (_rcmdjp._isObject(obj)) {
        var clone = {};
        for (var key in obj)
          if (obj.hasOwnProperty(key)) clone[key] = _rcmdjp._clone(obj[key]);
        return clone;
      } else if (_rcmdjp._isArray(obj)) {
        var clone = [];
        for (var i = 0, value; value = obj[i]; i++)
          clone[i] = _rcmdjp._clone(value);
        return clone;
      } else {
        return obj;
      }
    },
    _isObject: function(obj) {
      return obj === Object(obj);
    },
    _isArray: function(obj) {
      return obj instanceof Array;
    },

    // decoration
    _decorateTemplate: function (options) {
      if (!options['mode']) return;
      if (!options['selector']) return;
      if (!_rcmdjp._pluginURLs['jquery_' + options['mode']]) return;
      _rcmdjp._loadJquery(function ($) {
        var method = '_rcmdjp' + _rcmdjp._pascalCase(options['mode']),
          callback = function () {
            options._rcmdjp = {};
            options._rcmdjp.events = _rcmdjp._observedEvents;
            $(options['selector'])[method](options);
          };
        if ($.fn[method]) {
          callback();
          return;
        }
        _rcmdjp._loadLibrary(_rcmdjp._sslize(_rcmdjp._pluginURLs['jquery_' + options['mode']]), callback);
      });
    },
    _loadJquery: function (callback) {
      var _jQuery = window.jQuery, _$ = window.$;
      if (_rcmdjp._jQuery) {
        callback(_rcmdjp._jQuery);
        return;
      }
      _rcmdjp._loadLibrary(_rcmdjp._sslize("https://d.rcmd.jp/javascripts/jquery/1.11.0/jquery-1.11.0.min.js"), function () {
        if (!_rcmdjp._jQuery) _rcmdjp._jQuery = window.jQuery;
        window.jQuery = _jQuery;
        window.$ = _$;
        callback(_rcmdjp._jQuery);
      });
    },
    _loadLibrary: (function () {
      var src_callbacks = {};
      return function (src, callback) {
        if (!src_callbacks.hasOwnProperty(src)) {
          src_callbacks[src] = [];
          var script = document.createElement('script');
          script.type = 'text/javascript';
          script.charset = 'UTF-8';
          script.src = src;
          script.onload = function () {
            for (var i = 0, l = src_callbacks[src].length; i < l; i++) {
              src_callbacks[src][i]();
            }
          };
          document.body.appendChild(script);
        }
        src_callbacks[src].push(callback);
      };
    })(),

    // Compatible with different 「el」 type
    _adaptElementID: function(_elementID){
      if(typeof _elementID === 'string') {
        return document.getElementById(_elementID);
      }
      if(_elementID instanceof Element) {
        return _elementID;
      }
    },

    // settings
    _accountName: "marutsu.co.jp",
    _recommendName: "item",
    _multiDeviceHistoryURLs: {
      post: "https://history.rcmd.jp/history/save",
      get: "https://history.rcmd.jp/history/load"
    },
    _useMultiDeviceHistory: false,
    _pingURLs: {
      user: "https://d.rcmd.jp/user.gif",
      data: "https://d.rcmd.jp/item.gif",
      recommend: "https://d.rcmd.jp/iview.gif",
      track: "https://d.rcmd.jp/rview.gif",
      conversion: "https://d.rcmd.jp/cv.gif"
    },
    _jsonpURLs: {
      pv_recommend: "https://d.rcmd.jp/marutsu.co.jp/item/i/#{filter}/#{code}.js",
      cv_recommend: "https://d.rcmd.jp/marutsu.co.jp/item/c/#{filter}/#{code}.js",
      ranking: "https://d.rcmd.jp/marutsu.co.jp/item/r/#{type}/#{span}.js",
      category_ranking: "https://d.rcmd.jp/marutsu.co.jp/item/cr/#{category}/#{type}/#{span}.js",
      multi_category_ranking: "https://d.rcmd.jp/marutsu.co.jp/item/mcr/#{column}/#{category}/#{type}/#{span}.js",
      item: "https://d.rcmd.jp/marutsu.co.jp/item/item/#{code}.js",
      new_item: "https://d.rcmd.jp/marutsu.co.jp/item/ni/#{span}.js",
      category_new_item: "https://d.rcmd.jp/marutsu.co.jp/item/cni/#{category}/#{span}.js",
      multi_category_new_item: "https://d.rcmd.jp/marutsu.co.jp/item/mcni/#{column}/#{category}/#{span}.js",
      pickup: "https://d.rcmd.jp/marutsu.co.jp/item/pi/#{label}.js",
      personalized_recommend: "https://d.rcmd.jp/marutsu.co.jp/item/pr/#{type}/#{user}.js",
      pv_cv_recommend: "https://d.rcmd.jp/marutsu.co.jp/item/pv_cv/#{filter}/#{code}.js",
      img_recommend: "https://d.rcmd.jp/marutsu.co.jp/item/img/#{filter}/#{code}.js",
      text_recommend: "https://d.rcmd.jp/marutsu.co.jp/item/text/#{filter}/#{code}.js"
    },
    _itemCodeGetter: "",
    _templates: {
      "cv-recommend": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"cv-rcmd\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body\"\u003e\r\n  \u003ch3 class=\"title\"\u003eよく一緒に購入されている製品\u003c/h3\u003e\r\n  \u003cul class=\"recommend_rcmd_display\"\u003e\r\n    ");this._loop(0,5,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\"\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\" loading=\"lazy\"\u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",27);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n        \u003cdiv class=\"detail_btn\"\u003e\r\n        \u003ca href=\"");this._var("url");this._print("\" class=\"shohin_btn\"\u003e詳細を見る\u003c/a\u003e\r\n        \u003c/div\u003e\r\n      \u003c/a\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});}, "#cv-rcmd .recommend_rcmd_body {\r\n    clear: both;\r\n    padding-bottom: 1em;\r\n}\r\n\r\n#cv-rcmd .recommend_rcmd_body h3 {\r\n    width: 100%;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#cv-rcmd .recommend_rcmd_display {\r\n    clear: both;\r\n    display: flex;\r\n    width: 100%;\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#cv-rcmd .recommend_rcmd_display li {\r\n    width: 20%;\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    /*padding: 0 !important;*/\r\n    padding: 0 10px 30px !important;\r\n    background: none !important;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#cv-rcmd .recommend_rcmd_display li a {\r\n    display: block;\r\n    padding: 0.5em;\r\n}\r\n\r\n#cv-rcmd .recommend_rcmd_display li img {\r\n    display: block;\r\n    width: 150px;\r\n    height: 150px;\r\n    margin: 0 auto 10px;\r\n}\r\n\r\n#cv-rcmd .recommend_rcmd_display li p {\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n\r\n\r\n/* 追記 */\r\n\r\n#cv-rcmd .title{\r\n    font-size: 1.3em;\r\n    font-weight: bold;\r\n    padding: 0 5px 5px 5px;\r\n    border-bottom: 2px solid #aaa;\r\n    margin: 0 0 18px 0;\r\n}\r\n\r\n#cv-rcmd .rcmd_recommend_name{\r\n    margin: 10px 0 0!important;\r\n    font-weight: 800!important;\r\n}\r\n\r\n#cv-rcmd .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-weight: 800;\r\n    font-size: 1.2em!important;\r\n    margin: 0 auto 0 0;\r\n    line-height: 1em!important;\r\n}\r\n\r\n#cv-rcmd .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n#cv-rcmd .detail_btn{\r\n    width: 100%;\r\n    max-width: 140px;\r\n    flex-grow: 0;\r\n    flex-shrink: 0;\r\n    margin: 10px auto 0!important;\r\n}\r\n\r\n#cv-rcmd .shohin_btn {\r\n  padding: 7px 10px;\r\n  display: flex;\r\n  justify-content: center;\r\n  align-items: center;\r\n  background:#ffffff;\r\n  border: 1px solid #004390;\r\n  box-shadow: 0 2px 0 #ddd;\r\n  border-radius: 5px;\r\n  font-size: 14px;\r\n  font-weight: 700;\r\n  color: #004390;\r\n  text-decoration: none;\r\n  text-align: center;\r\n}\r\n\r\n#cv-rcmd .shohin_btn:before {\r\n  content: \"\";\r\n  font-family: icons;\r\n  text-rendering: auto;\r\n  -webkit-font-smoothing: antialiased;\r\n  -moz-osx-font-smoothing: grayscale;\r\n  margin-right: 5px;\r\n  font-weight: 400;\r\n  font-size: 9px;\r\n  transform: scale(.8) translateY(-1px);\r\n}\r\n\r\n#cv-rcmd .shohin_btn:hover {\r\n    opacity: 1;\r\n    box-shadow: inset 0 0 100px rgba(0, 0, 0, .15), inset 0 2px 2px rgba(0, 0, 0, .5);\r\n    transform: translateY(1px);\r\n}\r\n\r\n\r\n@media screen and (max-width: 1050px) {\r\n    #cv-rcmd .recommend_rcmd_display li {\r\n        width: 33%;\r\n    }\r\n}\r\n\r\n@media screen and (max-width: 600px) {\r\n    #cv-rcmd .recommend_rcmd_display li {\r\n        width: 50%;\r\n    }\r\n\r\n    /* SPサイトで非表示 */\r\n    #cv-rcmd .btn_cart{\r\n        display: none;\r\n     }\r\n}"],"newitem_top": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"ni_top\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body\"\u003e\r\n\u003ch3 class=\"h3head\"\u003e新着商品\u003c/h3\u003e\r\n  \u003cul class=\"recommend_rcmd_display\"\u003e\r\n    ");this._loop(0,5,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\"\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\" loading=\"lazy\" \u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",30);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n        \u003cdiv class=\"detail_btn\"\u003e\r\n        \u003ca href=\"");this._var("url");this._print("\" class=\"shohin_btn\"\u003e詳細を見る\u003c/a\u003e\r\n        \u003c/div\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});}, "#ni_top{\r\n    margin: 18px 0 0;\r\n}\r\n\r\n#ni_top .recommend_rcmd_body {\r\n    clear: both;\r\n    padding-bottom: 1em;\r\n}\r\n\r\n#ni_top .recommend_rcmd_body .h3head {\r\n    width: 100%;\r\n    font-size: 1.4em;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n    border-bottom: 1px solid #ccc;\r\n    font-weight: 800;\r\n}\r\n\r\n#ni_top .recommend_rcmd_display {\r\n    clear: both;\r\n    display: flex;\r\n    width: 100%;\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#ni_top .recommend_rcmd_display li {\r\n    width: 20%;\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    padding: 0 10px 30px !important;\r\n    background: none !important;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#ni_top .recommend_rcmd_display li a {\r\n    display: block;\r\n    padding: 0.5em;\r\n    text-decoration: none;\r\n}\r\n\r\n#ni_top .recommend_rcmd_display li img {\r\n    display: block;\r\n    width: 80px;\r\n    height: 80px;\r\n    margin: 0 auto 10px;\r\n}\r\n\r\n#ni_top .recommend_rcmd_display li p {\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n/* 追記 */\r\n\r\n#ni_top .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-weight: 900;\r\n    font-size: 1.4em!important;\r\n    margin: 0 auto 0 0;\r\n    line-height: 1em!important;\r\n}\r\n\r\n#ni_top .rcmd_recommend_price_sub{\r\n    color: #000000;\r\n    font-size: 0.9em;\r\n}\r\n\r\n\r\n#ni_top .detail_btn{\r\n    width: 100%;\r\n    max-width: 140px;\r\n    flex-grow: 0;\r\n    flex-shrink: 0;\r\n    margin: 6px auto 0!important;\r\n}\r\n\r\n#ni_top .shohin_btn {\r\n  padding: 7px 10px;\r\n  display: flex;\r\n  justify-content: center;\r\n  align-items: center;\r\n  background:#ffffff;\r\n  border: 1px solid #004390;\r\n  box-shadow: 0 2px 0 #ddd;\r\n  border-radius: 5px;\r\n  font-size: 14px;\r\n  font-weight: 700;\r\n  color: #004390;\r\n  text-decoration: none;\r\n  text-align: center;\r\n}\r\n\r\n#ni_top .shohin_btn:before {\r\n  content: \"\";\r\n  font-family: icons;\r\n  text-rendering: auto;\r\n  -webkit-font-smoothing: antialiased;\r\n  -moz-osx-font-smoothing: grayscale;\r\n  margin-right: 5px;\r\n  font-weight: 400;\r\n  font-size: 9px;\r\n  transform: scale(.8) translateY(-1px);\r\n}\r\n\r\n#ni_top .shohin_btn:hover {\r\nopacity: 1;\r\n    box-shadow: inset 0 0 100px rgba(0, 0, 0, .15), inset 0 2px 2px rgba(0, 0, 0, .5);\r\n    transform: translateY(1px);\r\n}\r\n\r\n#ni_top .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n\r\n@media screen and (max-width: 1050px) {\r\n    #ni_top .recommend_rcmd_display li {\r\n        width: 33%; \r\n    }\r\n}\r\n\r\n@media screen and (max-width: 800px) {\r\n    #ni_top .recommend_rcmd_display li {\r\n        width: 50%;\r\n    }\r\n}"],"personalized-recommend_cart": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"pr_cart\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body\"\u003e\r\n\u003ch3 class=\"h4head\"\u003e閲覧履歴に基づくおすすめ品\u003c/h3\u003e\r\n  \u003cul class=\"recommend_rcmd_display\"\u003e\r\n    ");this._loop(0,5,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\" target=\"_top\"\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\" loading=\"lazy\" \u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",31);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n        \u003cdiv class=\"detail_btn\"\u003e\r\n        \u003ca href=\"");this._var("url");this._print("\" class=\"shohin_btn\"\u003e詳細を見る\u003c/a\u003e\r\n        \u003c/div\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});}, "#pr_cart .recommend_rcmd_body {\r\n    clear: both;\r\n    padding-bottom: 1em;\r\n}\r\n\r\n#pr_cart .recommend_rcmd_body h3 {\r\n    width: 100%;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_cart .recommend_rcmd_display {\r\n    clear: both;\r\n    display: flex;\r\n    width: 100%;\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#pr_cart .recommend_rcmd_display li {\r\n    width: 20%;\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    padding: 0 15px 30px !important;\r\n    background: none !important;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_cart .recommend_rcmd_display li a {\r\n    display: block;\r\n    padding: 0.5em;\r\n}\r\n\r\n#pr_cart .recommend_rcmd_display li img {\r\n    display: block;\r\n    width: 100px;\r\n    height: 100px;\r\n    margin: 0 auto 10px;\r\n}\r\n\r\n#pr_cart .recommend_rcmd_display li p {\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n    font-size: 1.1em;\r\n    font-weight: 800;\r\n}\r\n\r\n/* 追記 */\r\n#pr_cart .h3title{\r\n    font-size: 1.2em;\r\n    font-weight: 800;\r\n    margin: 0 0 20px;\r\n    padding: 0 0 10px;\r\n    border-bottom: 1px solid #ccc;\r\n}\r\n\r\n#pr_cart .h4head{\r\n    font-size: 20px;\r\n    font-weight: bold;\r\n    color: white;\r\n    border-bottom: 1px solid #004390;\r\n    background-color: #004390;\r\n    padding: 6px 10px;\r\n    margin: 0 0 5px;\r\n}\r\n\r\n\r\n#pr_cart .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-weight: 900;\r\n    font-size: 1.2em!important;\r\n    margin: 0 auto 0 0!important;\r\n}\r\n\r\n#pr_cart .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-size: 1.4em!important;\r\n    margin: 0 auto 0 0;\r\n    line-height: 1em!important;\r\n}\r\n\r\n#pr_cart .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n#pr_cart .detail_btn{\r\n    width: 100%;\r\n    max-width: 140px;\r\n    flex-grow: 0;\r\n    flex-shrink: 0;\r\n    margin: 12px auto 0!important;\r\n}\r\n\r\n#pr_cart .shohin_btn {\r\n  padding: 7px 10px;\r\n  display: flex;\r\n  justify-content: center;\r\n  align-items: center;\r\n  background:#ffffff;\r\n  border: 1px solid #004390;\r\n  box-shadow: 0 2px 0 #ddd;\r\n  border-radius: 5px;\r\n  font-size: 14px;\r\n  font-weight: 700;\r\n  color: #004390;\r\n  text-decoration: none;\r\n  text-align: center;\r\n}\r\n\r\n#pr_cart .shohin_btn:before {\r\n  content: \"\";\r\n  font-family: icons;\r\n  text-rendering: auto;\r\n  -webkit-font-smoothing: antialiased;\r\n  -moz-osx-font-smoothing: grayscale;\r\n  margin-right: 5px;\r\n  font-weight: 400;\r\n  font-size: 9px;\r\n  transform: scale(.8) translateY(-1px);\r\n}\r\n\r\n#pr_cart .shohin_btn:hover {\r\n    opacity: 1;\r\n    box-shadow: inset 0 0 100px rgba(0, 0, 0, .15), inset 0 2px 2px rgba(0, 0, 0, .5);\r\n    transform: translateY(1px);\r\n}\r\n\r\n\r\n/*@media screen and (max-width: 1100px) {\r\n    #pr_cart .recommend_rcmd_display li {\r\n        width: 33%; \r\n    }\r\n}*/\r\n\r\n@media screen and (max-width: 680px) {\r\n    #pr_cart .recommend_rcmd_display li {\r\n        width: 50%;\r\n    }\r\n\r\n    #pr_cart .btn_cart{\r\n    display: none;\r\n    }\r\n}\r\n"],"personalized-recommend_myp": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"pr_myp\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body\"\u003e\r\n\u003ch4 class=\"h4head\"\u003e閲覧履歴に基づくおすすめ品\u003c/h4\u003e\r\n  \u003cul class=\"recommend_rcmd_display\"\u003e\r\n    ");this._loop(0,5,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\" target=\"_top\"\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\" loading=\"lazy\" \u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",26);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n        \u003cdiv class=\"detail_btn\"\u003e\r\n        \u003ca href=\"");this._var("url");this._print("\" class=\"shohin_btn\"\u003e詳細を見る\u003c/a\u003e\r\n        \u003c/div\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});}, "#pr_myp .recommend_rcmd_body {\r\n    clear: both;\r\n    padding-bottom: 1em;\r\n}\r\n\r\n#pr_myp .recommend_rcmd_body h3 {\r\n    width: 100%;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_myp .recommend_rcmd_display {\r\n    clear: both;\r\n    display: flex;\r\n    width: 100%;\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#pr_myp .recommend_rcmd_display li {\r\n    width: 20%;\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    padding: 0 10px 30px !important;\r\n    background: none !important;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_myp .recommend_rcmd_display li a {\r\n    display: block;\r\n    padding: 0.5em;\r\n}\r\n\r\n#pr_myp .recommend_rcmd_display li img {\r\n    display: block;\r\n    width: 100px;\r\n    height: 100px;\r\n    margin: 0 auto 10px;\r\n}\r\n\r\n#pr_myp .recommend_rcmd_display li p {\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n    line-height: 1.4em\r\n}\r\n\r\n\r\n/* 追記 */\r\n#pr_myp .h3title{\r\n    font-size: 1.2em;\r\n    font-weight: 800;\r\n    margin: 0 0 20px;\r\n    padding: 0 0 10px;\r\n    border-bottom: 1px solid #ccc;\r\n}\r\n\r\n\r\n#pr_myp .h4head{\r\n    background-color: #f5f5f5;\r\n    padding: 6px 10px;\r\n    margin: 0 0 18px;\r\n    font-weight: bold;\r\n}\r\n\r\n\r\n#pr_myp .rcmd_recommend_name{\r\n   font-size: 1.1em;\r\n   font-weight: 800;\r\n}\r\n\r\n#pr_myp .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-weight: 900;\r\n    font-size: 1.3em;\r\n    margin: 0 auto 0 0;\r\n}\r\n\r\n#pr_myp .detail_btn{\r\n    width: 100%;\r\n    max-width: 140px;\r\n    flex-grow: 0;\r\n    flex-shrink: 0;\r\n    margin: 6px auto 0!important;\r\n}\r\n\r\n#pr_myp .shohin_btn {\r\n  padding: 7px 10px;\r\n  display: flex;\r\n  justify-content: center;\r\n  align-items: center;\r\n  background:#ffffff;\r\n  border: 1px solid #004390;\r\n  box-shadow: 0 2px 0 #ddd;\r\n  border-radius: 5px;\r\n  font-size: 14px;\r\n  font-weight: 700;\r\n  color: #004390;\r\n  text-decoration: none;\r\n  text-align: center;\r\n}\r\n\r\n#pr_myp .shohin_btn:before {\r\n  content: \"\";\r\n  font-family: icons;\r\n  text-rendering: auto;\r\n  -webkit-font-smoothing: antialiased;\r\n  -moz-osx-font-smoothing: grayscale;\r\n  margin-right: 5px;\r\n  font-weight: 400;\r\n  font-size: 9px;\r\n  transform: scale(.8) translateY(-1px);\r\n}\r\n\r\n#pr_myp .shohin_btn:hover {\r\nopacity: 1;\r\n    box-shadow: inset 0 0 100px rgba(0, 0, 0, .15), inset 0 2px 2px rgba(0, 0, 0, .5);\r\n    transform: translateY(1px);\r\n}\r\n\r\n\r\n#pr_myp .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-size: 1.4em!important;\r\n    margin: 0 auto 0 0;\r\n    line-height: 1em!important;\r\n}\r\n\r\n#pr_myp .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n@media screen and (max-width: 1100px) {\r\n    #pr_myp .recommend_rcmd_display li {\r\n        width: 33%; \r\n    }\r\n}\r\n\r\n@media screen and (max-width: 800px) {\r\n    #pr_myp .recommend_rcmd_display li {\r\n        width: 50%;\r\n    }\r\n\r\n    #pr_myp .btn_cart{\r\n    display: none;\r\n    }\r\n}\r\n"],"personalized-recommend_rate": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"pr_rate\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body\"\u003e\r\n\u003ch3 class=\"h3title\"\u003e閲覧履歴に基づくおすすめ品\u003c/h3\u003e\r\n  \u003cul class=\"recommend_rcmd_display\"\u003e\r\n    ");this._loop(0,10,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\" target=\"_top\"\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\" loading=\"lazy\" \u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",28);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n        \u003cdiv class=\"detail_btn\"\u003e\r\n        \u003ca href=\"");this._var("url");this._print("\" class=\"shohin_btn\"\u003e詳細を見る\u003c/a\u003e\r\n        \u003c/div\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});}, "#pr_rate{\r\n   margin: 30px 0 0;\r\n}\r\n\r\n#pr_rate .recommend_rcmd_body {\r\n    clear: both;\r\n    padding-bottom: 1em;\r\n}\r\n\r\n#pr_rate .recommend_rcmd_body h3 {\r\n    width: 100%;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_rate .recommend_rcmd_display {\r\n    clear: both;\r\n    display: flex;\r\n    width: 100%; /* 100% */\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: auto!important;\r\n    margin-bottom: 0;\r\n    margin-left: auto!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#pr_rate .recommend_rcmd_display li {\r\n    width: 10%; /* 20% */\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    padding: 0 15px 30px !important;\r\n    background: none !important;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_rate .recommend_rcmd_display li a {\r\n    display: block;\r\n    padding: 0.5em;\r\n}\r\n\r\n#pr_rate .recommend_rcmd_display li img {\r\n    display: block;\r\n    width: 100px;\r\n    height: 100px;\r\n    margin: 0 auto 10px;\r\n}\r\n\r\n#pr_rate .recommend_rcmd_display li p {\r\n    margin-top: 5px;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n    font-size: 1.1em;\r\n    font-weight: 800;\r\n    line-height: 1.7em;\r\n}\r\n\r\n\r\n/* 追記 */\r\n#pr_rate .h3title{\r\n    font-size: 1.8em;\r\n    font-weight: 800;\r\n    margin: 0 0 20px!important;\r\n    padding: 0 0 10px!important;\r\n    border-bottom: 1px solid #ccc;\r\n}\r\n\r\n#pr_rate .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-size: 1.4em!important;\r\n    margin: 0 auto 0 0;\r\n    line-height: 1em!important;\r\n}\r\n\r\n#pr_rate .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n#pr_rate .detail_btn{\r\n    width: 100%;\r\n    max-width: 140px;\r\n    flex-grow: 0;\r\n    flex-shrink: 0;\r\n    margin: 6px auto 0!important;\r\n}\r\n\r\n#pr_rate .shohin_btn {\r\n  padding: 7px 10px;\r\n  display: flex;\r\n  justify-content: center;\r\n  align-items: center;\r\n  background:#ffffff;\r\n  border: 1px solid #004390;\r\n  box-shadow: 0 2px 0 #ddd;\r\n  border-radius: 5px;\r\n  font-size: 14px;\r\n  font-weight: 700;\r\n  color: #004390;\r\n  text-decoration: none;\r\n  text-align: center;\r\n}\r\n\r\n#pr_rate .shohin_btn:before {\r\n  content: \"\";\r\n  font-family: icons;\r\n  text-rendering: auto;\r\n  -webkit-font-smoothing: antialiased;\r\n  -moz-osx-font-smoothing: grayscale;\r\n  margin-right: 5px;\r\n  font-weight: 400;\r\n  font-size: 9px;\r\n  transform: scale(.8) translateY(-1px);\r\n}\r\n\r\n#pr_rate .shohin_btn:hover {\r\nopacity: 1;\r\n    box-shadow: inset 0 0 100px rgba(0, 0, 0, .15), inset 0 2px 2px rgba(0, 0, 0, .5);\r\n    transform: translateY(1px);\r\n}\r\n\r\n\r\n@media screen and (max-width: 1500px) {\r\n    #pr_rate .recommend_rcmd_display li {\r\n        width: 20%;\r\n    }\r\n\r\n    */#pr_rate .recommend_rcmd_display li img {\r\n        width: 100px;\r\n     }*/\r\n\r\n    #pr_rate .btn_cart{\r\n    display: none;\r\n    }\r\n}\r\n\r\n\r\n@media screen and (max-width: 800px) {\r\n    #pr_rate .recommend_rcmd_display li {\r\n        width: 50%;\r\n    }\r\n}\r\n"],"personalized-recommend_side": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"pr_side\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body\"\u003e\r\n\u003ch3 class=\"ttl_h3\"\u003eおすすめ商品\u003c/h3\u003e\r\n  \u003cul class=\"recommend_rcmd_display\"\u003e\r\n    ");this._loop(0,10,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\" target=\"_top\"\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\"  loading=\"lazy\"\u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",20);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n        \u003cdiv class=\"detail_btn\"\u003e\r\n        \u003ca href=\"");this._var("url");this._print("\" class=\"shohin_btn\"\u003e詳細を見る\u003c/a\u003e\r\n        \u003c/div\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});}, "#pr_side{\r\n   width: 260px;\r\n   margin: 20px 0 0;\r\n}\r\n\r\n#pr_side .recommend_rcmd_body {\r\n    clear: both;\r\n    padding-bottom: 1em;\r\n}\r\n\r\n#pr_side .recommend_rcmd_body h3 {\r\n    width: 100%;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_side .recommend_rcmd_display {\r\n    clear: both;\r\n    display: flex;\r\n    width: 100%;\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#pr_side .recommend_rcmd_display li {\r\n    width: 50%;\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    padding: 0 10px 20px !important;\r\n    background: none !important;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_side .recommend_rcmd_display li a {\r\n    display: block;\r\n    padding: 0.35em; /* 0.5em */\r\n}\r\n\r\n#pr_side .recommend_rcmd_display li img {\r\n    display: block;\r\n    width: 70px; /* 100% */\r\n    height: 70px;\r\n    margin: 0 auto 8px;\r\n}\r\n\r\n#pr_side .recommend_rcmd_display li p {\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n    line-height: 1.3em;\r\n    font-weight: 800;\r\n}\r\n\r\n\r\n/* 追記 */\r\n#pr_side .ttl_h3{\r\n   font-size: 1.2em;\r\n   font-weight: 800;\r\n   margin: 0 0 20px;\r\n   padding: 0 0 10px;\r\n   border-bottom: 1px solid #ccc;\r\n}\r\n\r\n#pr_side .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-size: 1.2em!important;\r\n    margin: 0 auto 0 0;\r\n    line-height: 1em!important;\r\n}\r\n\r\n#pr_side .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n#pr_side .detail_btn{\r\n    width: 110px;\r\n    flex-grow: 0;\r\n    flex-shrink: 0;\r\n    margin: 6px auto 0!important;\r\n}\r\n\r\n#pr_side .shohin_btn {\r\n  padding: 7px 10px;\r\n  display: flex;\r\n  justify-content: center;\r\n  align-items: center;\r\n  background:#ffffff;\r\n  border: 1px solid #004390;\r\n  box-shadow: 0 2px 0 #ddd;\r\n  border-radius: 5px;\r\n  font-size: 0.9em;\r\n  font-weight: 700;\r\n  color: #004390;\r\n  text-decoration: none;\r\n  text-align: center;\r\n}\r\n\r\n#pr_side .shohin_btn:before {\r\n  content: \"\";\r\n  font-family: icons;\r\n  text-rendering: auto;\r\n  -webkit-font-smoothing: antialiased;\r\n  -moz-osx-font-smoothing: grayscale;\r\n  margin-right: 5px;\r\n  font-weight: 400;\r\n  font-size: 9px;\r\n  transform: scale(.8) translateY(-1px);\r\n}\r\n\r\n#pr_side .shohin_btn:hover {\r\n    opacity: 1;\r\n    box-shadow: inset 0 0 100px rgba(0, 0, 0, .15), inset 0 2px 2px rgba(0, 0, 0, .5);\r\n    transform: translateY(1px);\r\n}\r\n\r\n\r\n@media screen and (max-width: 420px) {\r\n    #pr_side .recommend_rcmd_display li {\r\n        width: 50%;\r\n    }\r\n\r\n}\r\n"],"personalized-recommend_top": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"pr_top\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body\"\u003e\r\n\u003ch2 class=\"section_title\"\u003e閲覧履歴に基づくおすすめ品\u003c/h2\u003e\r\n  \u003cul class=\"recommend_rcmd_display\"\u003e\r\n    ");this._loop(0,5,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\" target=\"_top\"\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\"  loading=\"lazy\"\u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",27);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n        \u003cdiv class=\"detail_btn\"\u003e\r\n        \u003ca href=\"");this._var("url");this._print("\" class=\"shohin_btn\"\u003e詳細を見る\u003c/a\u003e\r\n        \u003c/div\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});}, "#pr_top{\r\n   width: 1240px;\r\n   margin: 10px auto 0;\r\n}\r\n\r\n#pr_top .recommend_rcmd_body {\r\n    clear: both;\r\n    padding-bottom: 1em;\r\n    background-color: #ffffff;\r\n}\r\n\r\n#pr_top .recommend_rcmd_body h3 {\r\n    width: 100%;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_top .recommend_rcmd_display {\r\n    clear: both;\r\n    display: flex;\r\n    width: 1050px;/* 100% */\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: auto!important;\r\n    margin-bottom: 0;\r\n    margin-left: auto!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#pr_top .recommend_rcmd_display li {\r\n    width: 210px;\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    padding: 0 15px 30px !important;\r\n    background: none !important;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pr_top .recommend_rcmd_display li a {\r\n    display: block;\r\n    padding: 0.5em;\r\n    text-decoration: none;\r\n}\r\n\r\n#pr_top .recommend_rcmd_display li img {\r\n    display: block;\r\n    width: 170px;\r\n    height: 170px;\r\n    margin: 0 auto 10px;\r\n}\r\n\r\n#pr_top .recommend_rcmd_display li p {\r\n    margin-top: 6px; /* 10px */\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n    font-size: 1.1em;\r\n    font-weight: 800;\r\n    line-height: 1.7em;\r\n}\r\n\r\n\r\n/* 追記 */\r\n#pr_top .h3title{\r\n    font-size: 1.2em;\r\n    font-weight: 800;\r\n    margin: 0 0 20px;\r\n    padding: 0 0 10px;\r\n    border-bottom: 1px solid #ccc;\r\n}\r\n\r\n#pr_top .section_title{\r\n    margin: 30px 0;\r\n    padding-bottom: 15px;\r\n    font-weight: 700;\r\n    font-size: 26px;\r\n    line-height: 1.2;\r\n    border-bottom: 1px solid #e5e5e5;\r\n}\r\n\r\n#pr_top .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-size: 1.4em!important;\r\n    margin: 0 auto 0 0;\r\n    line-height: 1em!important;\r\n}\r\n\r\n\r\n#pr_top .detail_btn{\r\n    width: 140px;\r\n    flex-grow: 0;\r\n    flex-shrink: 0;\r\n    margin: 8px 0 0!important;\r\n}\r\n\r\n#pr_top .shohin_btn {\r\n  padding: 7px 10px;\r\n  display: flex;\r\n  justify-content: center;\r\n  align-items: center;\r\n  background:#ffffff;\r\n  border: 1px solid #004390;\r\n  box-shadow: 0 2px 0 #ddd;\r\n  border-radius: 5px;\r\n  font-size: 14px;\r\n  font-weight: 700;\r\n  color: #004390;\r\n  text-decoration: none;\r\n  text-align: center;\r\n}\r\n\r\n#pr_top .shohin_btn:before {\r\n  content: \"\";\r\n  font-family: icons;\r\n  text-rendering: auto;\r\n  -webkit-font-smoothing: antialiased;\r\n  -moz-osx-font-smoothing: grayscale;\r\n  margin-right: 5px;\r\n  font-weight: 400;\r\n  font-size: 9px;\r\n  transform: scale(.8) translateY(-1px);\r\n}\r\n\r\n#pr_top .shohin_btn:hover {\r\nopacity: 1;\r\n    box-shadow: inset 0 0 100px rgba(0, 0, 0, .15), inset 0 2px 2px rgba(0, 0, 0, .5);\r\n    transform: translateY(1px);\r\n}\r\n\r\n#pr_top .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n@media screen and (max-width: 736px) {\r\n\r\n    #pr_top{\r\n        width: 100%;\r\n        max-width: 736px;\r\n    }\r\n\r\n    #pr_top .recommend_rcmd_display{\r\n        width: 100%;\r\n    }\r\n\r\n    #pr_top .recommend_rcmd_display li {\r\n        width: 50%;\r\n    }\r\n\r\n    #pr_top .btn_cart{\r\n    display: none;\r\n    }\r\n}\r\n"],"pv-ranking_side": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"pvr_side\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body_pvr\"\u003e\r\n\u003ch3 class=\"ttl_h3\"\u003e人気の製品\u003c/h3\u003e\r\n  \u003cul class=\"recommend_rcmd_display_pvr\"\u003e\r\n    ");this._loop(0,10,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\" target=\"_top\"\u003e\r\n        \u003c!--\u003cspan class=\"rank\"\u003e");this._var("num");this._print("\u003c/span\u003e--\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\" loading=\"lazy\"\u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",25);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n        \u003cdiv class=\"detail_btn\"\u003e\r\n        \u003ca href=\"");this._var("url");this._print("\" class=\"shohin_btn\"\u003e詳細を見る\u003c/a\u003e\r\n        \u003c/div\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});}, "#pvr_side{\r\n    width: 260px!important;\r\n}\r\n\r\n#pvr_side .recommend_rcmd_body_pvr {\r\n    clear: both;\r\n    padding-bottom: 1em;\r\n}\r\n\r\n#pvr_side .recommend_rcmd_body_pvr h3 {\r\n    width: 100%;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pvr_side .recommend_rcmd_display_pvr {\r\n    clear: both;\r\n    display: flex;\r\n    width: 100%;\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#pvr_side .recommend_rcmd_display_pvr li {\r\n    width: 50%!important;\r\n    padding: 0 7px 20px !important;\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    background: none !important;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pvr_side .recommend_rcmd_display_pvr li a {\r\n    display: block;\r\n    padding: 0.35em;/* 0.5em; */\r\n    position: relative;\r\n}\r\n\r\n#pvr_side .recommend_rcmd_display_pvr li a span.rank {\r\n    display: block;\r\n    position: absolute;\r\n    left: 0.5em;\r\n    top: 0.5em;\r\n    width: 25px;\r\n    height: 25px;\r\n    /*padding: 0.5em;*/\r\n    background: #004390;\r\n    color: #fff;\r\n    /*font-weight: bold;*/\r\n    font-size: 1.1em;\r\n    text-align: center;\r\n}\r\n\r\n#pvr_side .recommend_rcmd_display_pvr li img {\r\n    display: block;\r\n    width: 74px;\r\n    height: 74px;\r\n    margin: 0 auto 10px;\r\n}\r\n\r\n#pvr_side .recommend_rcmd_display_pvr li p {\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n    line-height: 1.3em;\r\n    font-weight: 800;\r\n}\r\n\r\n\r\n/* 追記 */\r\n#pvr_side .ttl_h3{\r\n   font-size: 1.2em;\r\n   font-weight: 800;\r\n   margin: 0 0 20px;\r\n   padding: 0 0 10px;\r\n   border-bottom: 1px solid #ccc;\r\n}\r\n\r\n#pvr_side .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-weight: 900;\r\n    font-size: 1.2em!important;\r\n    margin: 0 auto 0 0;\r\n    line-height: 1em!important;\r\n}\r\n\r\n#pvr_side .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n#pvr_side .detail_btn{\r\n    width: 110px;\r\n    flex-grow: 0;\r\n    flex-shrink: 0;\r\n    margin: 6px auto 0!important;\r\n}\r\n\r\n#pvr_side .shohin_btn {\r\n  padding: 7px 10px;\r\n  display: flex;\r\n  justify-content: center;\r\n  align-items: center;\r\n  background:#ffffff;\r\n  border: 1px solid #004390;\r\n  box-shadow: 0 2px 0 #ddd;\r\n  border-radius: 5px;\r\n  font-size: 0.9em;\r\n  font-weight: 700;\r\n  color: #004390;\r\n  text-decoration: none;\r\n  text-align: center;\r\n}\r\n\r\n#pvr_side .shohin_btn:before {\r\n  content: \"\";\r\n  font-family: icons;\r\n  text-rendering: auto;\r\n  -webkit-font-smoothing: antialiased;\r\n  -moz-osx-font-smoothing: grayscale;\r\n  margin-right: 5px;\r\n  font-weight: 400;\r\n  font-size: 9px;\r\n  transform: scale(.8) translateY(-1px);\r\n}\r\n\r\n#pvr_side .shohin_btn:hover {\r\n    opacity: 1;\r\n    box-shadow: inset 0 0 100px rgba(0, 0, 0, .15), inset 0 2px 2px rgba(0, 0, 0, .5);\r\n    transform: translateY(1px);\r\n}\r\n\r\n"],"pv-ranking_top": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"pvr_top\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body_pvr\"\u003e\r\n\u003ch2 id=\"main-recommend-title\"\"\u003eよく見られている製品\u003c/h2\u003e\r\n  \u003cul class=\"recommend_rcmd_display_pvr\"\u003e\r\n    ");this._loop(0,5,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\" target=\"_top\"\u003e\r\n        \u003cspan class=\"rank\"\u003e");this._var("num");this._print("\u003c/span\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\"  loading=\"lazy\"\u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",29);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\r\n         \u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});this._print("\r\n");}, "#pvr_top .recommend_rcmd_body_pvr {\r\n    clear: both;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_body_pvr h3 {\r\n    width: 100%;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_display_pvr {\r\n    clear: both;\r\n    display: flex;\r\n    width: 100%;\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_display_pvr li {\r\n    width: 20%;\r\n    padding: 0 8px 20px !important;\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    box-sizing: border-box;\r\n    background: #ffffff !important;\r\n    min-height: 180px;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_display_pvr li a {\r\n    display: block;\r\n    padding: 0.3em;\r\n    position: relative;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_display_pvr li a span.rank {\r\n    display: block;\r\n    position: absolute;\r\n    left: 0.5em;\r\n    top: 0.5em;\r\n    width: 25px;\r\n    height: 25px;\r\n    /*padding: 0.5em;*/\r\n    background: #004390;\r\n    color: #fff;\r\n    /*font-weight: bold;*/\r\n    font-size: 1.1em;\r\n    text-align: center;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_display_pvr li img {\r\n    display: block;\r\n    width: 90px;\r\n    height: 90px;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_display_pvr li p {\r\n    margin: 1px 0 !important;\r\n    line-height: 1.15em !important;\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n\r\n/* 追記 */\r\nh2#main-recommend-title {\r\n    color: #fff;\r\n    font-size: 16px !important;   /* 少しだけ小さく */\r\n    font-weight: 700;\r\n    line-height: 1.26;\r\n    margin-top: 0px !important;\r\n    margin-bottom: 6px !important;\r\n    margin-left: 5px;\r\n}\r\n\r\n#pvr_top .ttl_h3{\r\n   color: #ffffff;\r\n   font-size: 1.2em;\r\n   font-weight: 800;\r\n   margin: 0 0 20px;\r\n   padding: 0 0 10px;\r\n   border-bottom: 1px solid #ccc;\r\n}\r\n\r\n#pvr_top .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-weight: 900;\r\n    font-size: 1.1em;\r\n    margin: 0 auto 0 0;\r\n    line-height: 0.8em;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_display {\r\n    width: 1024px!important;\r\n    margin: 0 auto 0!important;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_display li{\r\n     width: 167px!important;\r\n}\r\n\r\n#pvr_top .recommend_rcmd_display li p{\r\n    font-size: 0.85em;\r\n    line-height: 1.4em;\r\n}\r\n\r\n#pvr_top .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n@media screen and (max-width: 736px) {\r\n \r\n     /* #pvr_top{\r\n      width: 100%;\r\n      max-width: 736px;\r\n    }\r\n\r\n    #pvr_top .recommend_rcmd_display_pvr li {\r\n     padding: 0 8px 10px !important;\r\n    }\r\n\r\n    #pvr_top .recommend_rcmd_display_pvr li:nth-child(n+4){\r\n      display: none;\r\n    }*/\r\n\r\n   #pvr_top .recommend_rcmd_body_pvr{\r\n     overflow-x: scroll;\r\n     overflow-y: hidden;\r\n   }\r\n\r\n   #pvr_top .recommend_rcmd_display_pvr{\r\n     flex-wrap: nowrap!important;\r\n     display: -webkit-box;\r\n}\r\n\r\n   #pvr_top .recommend_rcmd_display_pvr li {\r\n     width: 33%;\r\n    }\r\n\r\n\r\n}\r\n"],"pv-recommend": [function(){this._if("total","0",function(){this._print("\r\n");},function(){this._print("\r\n\u003cdiv id=\"pv-rcmd\"\u003e\r\n\u003cdiv class=\"recommend_rcmd_body\"\u003e\r\n  \u003ch3 class=\"title\"\u003eよく一緒に比較されている製品\u003c/h3\u003e\r\n  \u003cul class=\"recommend_rcmd_display\"\u003e\r\n    ");this._loop(0,5,function(){this._print("\r\n    \u003cli\u003e\r\n      \u003ca href=\"");this._var("url");this._print("\"\u003e\r\n        \u003cimg src=\"");this._var("image");this._print("\" alt=\"");this._var("name");this._print("\" onError=\"this.onerror=null;this.src='https://www.marutsu.co.jp/contents/shop/marutsu/img/common/noimage.jpg';\" loading=\"lazy\"\u003e\r\n        \u003cp class=\"rcmd_recommend_name\"\u003e");this._truncate("name",27);this._print("\u003c/p\u003e\r\n      \u003c/a\u003e\r\n        \u003cp class=\"rcmd_recommend_price\"\u003e￥");this._comma("price_2");this._print("\u003cbr /\u003e\u003cspan class=\"zeikomi\"\u003e税込￥");this._comma("price");this._print("\u003c/span\u003e\u003c/p\u003e\r\n        \u003cdiv class=\"detail_btn\"\u003e\r\n        \u003ca href=\"");this._var("url");this._print("\" class=\"shohin_btn\"\u003e詳細を見る\u003c/a\u003e\r\n        \u003c/div\u003e\r\n    \u003c/li\u003e\r\n    ");});this._print("\r\n  \u003c/ul\u003e\r\n\u003c/div\u003e\r\n\u003c/div\u003e\r\n");});}, "#pv-rcmd .recommend_rcmd_body {\r\n    clear: both;\r\n    padding-bottom: 1em;\r\n}\r\n\r\n#pv-rcmd .recommend_rcmd_body h3 {\r\n    width: 100%;\r\n    margin: 0 0 0.5em;\r\n    padding: 0.2em 0.5em;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pv-rcmd .recommend_rcmd_display {\r\n    clear: both;\r\n    display: flex;\r\n    width: 100%;\r\n    justify-content: start;\r\n    flex-wrap: wrap;\r\n    list-style: none;\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n#pv-rcmd .recommend_rcmd_display li {\r\n    width: 20%;\r\n    list-style: none;\r\n    margin: 0 !important;\r\n    /*padding: 0 !important;*/\r\n    padding: 0 10px 30px !important;\r\n    background: none !important;\r\n    box-sizing: border-box;\r\n}\r\n\r\n#pv-rcmd .recommend_rcmd_display li a {\r\n    display: block;\r\n    padding: 0.5em;\r\n}\r\n\r\n#pv-rcmd .recommend_rcmd_display li img {\r\n    display: block;\r\n    width: 150px;\r\n    height: 150px;\r\n    margin: 0 auto 10px;\r\n}\r\n\r\n#pv-rcmd .recommend_rcmd_display li p {\r\n    margin-top: 0;\r\n    margin-right: 0 !important;\r\n    margin-bottom: 0 !important;\r\n    margin-left: 0!important;\r\n    padding-top: 0;\r\n    padding-right: 0;\r\n    padding-bottom: 0;\r\n    padding-left: 0;\r\n}\r\n\r\n\r\n/* 追記 */\r\n\r\n#pv-rcmd .title{\r\n    font-size: 1.3em;\r\n    font-weight: bold;\r\n    padding: 0 5px 5px 5px;\r\n    border-bottom: 2px solid #aaa;\r\n    margin: 0 0 18px 0;\r\n}\r\n\r\n#pv-rcmd .rcmd_recommend_name{\r\n    margin: 10px 0 0!important;\r\n    font-weight: 800!important;\r\n}\r\n\r\n#pv-rcmd .rcmd_recommend_price{\r\n    color: #cc0000;\r\n    font-size: 1.2em!important;\r\n    margin: 0 auto 0 0;\r\n    line-height: 1em!important;\r\n    font-weight: 800;\r\n}\r\n\r\n#pv-rcmd .zeikomi{\r\n    font-size: 0.7em;\r\n    color: #000000!important;\r\n    font-weight: normal;\r\n}\r\n\r\n#pv-rcmd .detail_btn{\r\n    width: 100%;\r\n    max-width: 140px;\r\n    flex-grow: 0;\r\n    flex-shrink: 0;\r\n    margin: 10px auto 0!important;\r\n}\r\n\r\n#pv-rcmd .shohin_btn {\r\n  padding: 7px 10px;\r\n  display: flex;\r\n  justify-content: center;\r\n  align-items: center;\r\n  background:#ffffff;\r\n  border: 1px solid #004390;\r\n  box-shadow: 0 2px 0 #ddd;\r\n  border-radius: 5px;\r\n  font-size: 14px;\r\n  font-weight: 700;\r\n  color: #004390;\r\n  text-decoration: none;\r\n  text-align: center;\r\n}\r\n\r\n#pv-rcmd .shohin_btn:before {\r\n  content: \"\";\r\n  font-family: icons;\r\n  text-rendering: auto;\r\n  -webkit-font-smoothing: antialiased;\r\n  -moz-osx-font-smoothing: grayscale;\r\n  margin-right: 5px;\r\n  font-weight: 400;\r\n  font-size: 9px;\r\n  transform: scale(.8) translateY(-1px);\r\n}\r\n\r\n#pv-rcmd .shohin_btn:hover {\r\n    opacity: 1;\r\n    box-shadow: inset 0 0 100px rgba(0, 0, 0, .15), inset 0 2px 2px rgba(0, 0, 0, .5);\r\n    transform: translateY(1px);\r\n}\r\n\r\n\r\n@media screen and (max-width: 1050px) {\r\n    #pv-rcmd .recommend_rcmd_display li {\r\n        width: 33%;\r\n    }\r\n}\r\n\r\n@media screen and (max-width: 600px) {\r\n    #pv-rcmd .recommend_rcmd_display li {\r\n        width: 50%;\r\n    }\r\n\r\n    /* SPサイトで非表示 */\r\n    #pv-rcmd .btn_cart{\r\n        display: none;\r\n     }\r\n\r\n}"]
    },
    _ignoreReferes: [],
    _savedHistoryByte: {'view': 1024, 'click': 750, 'cv': 250},
    _displayedHistoryNum: 10,
    _displayedRecommendNum: 10,
    _displayedRankingNum: 10,
    _displayedNewItemNum: 10,
    _displayedPickupNum: 10,
    _displayedPersonalizedNum: 10,
    _combineFetchNum: 3,
    _disabledCodes: [],
    _cookieDomain: ".marutsu.co.jp",
    _invalidDomain: "",
    _pluginURLs: {"jquery_carousel":"https://d.rcmd.jp/javascripts/jquery/carousel/carousel.js","jquery_linear_carousel":"https://d.rcmd.jp/javascripts/jquery/linear-carousel/linear-carousel.js","jquery_thumbnail":"https://d.rcmd.jp/javascripts/jquery/thumbnail/thumbnail.js","jquery_collapse":"https://d.rcmd.jp/javascripts/jquery/collapse/collapse.js"},
    _cname: "",
    _allSecure: false
  };

  // template renderer
  _rcmdjp._Renderer = function() {};
  _rcmdjp._Renderer.prototype = {
    _clone: function() {
      var clone = new _rcmdjp._Renderer();
      clone._result = this._result;
      clone._items = this._items;
      clone._item = this._item;
      clone._globals = this._globals;
      clone._scripts = this._scripts;
      return clone;
    },
    _render: function(targetItem, items, func) {
      this._result = '';
      this._items = this._removeDisabledItems(items);
      this._item = {};
      this._globals = { total: items.length };
      if (targetItem)
        for (var column in targetItem) this._globals['target_' + column] = targetItem[column];
      this._scripts = [];
      for (var i = 0, item; item = this._items[i]; i++) item['num'] = i + 1;
      func.call(this);
      return this._result;
    },
    _removeDisabledItems: function(items) {
      var disabled = {};
      for (var i = 0, code; code = _rcmdjp._disabledCodes[i]; i++) disabled[code] = true;
      var results = [];
      for (var j = 0, item; item = items[j]; j++)
        if (!(item['code'] in disabled)) results.push(item);
      return results;
    },
    _evalScripts: function() {
      for (var i = 0, content; content = this._scripts[i]; i++) {
        try { eval(content); } catch (err) {}
      }
    },
    _print: function(result) {
      this._result += result;
    },
    _var: function(column) {
      this._print(this._toString(this._value(column)));
    },
    _if: function(column, expected, if_func, else_func) {
      if (expected == null ? this._toBoolean(this._value(column)) : this._equals(this._value(column), expected))
        if_func.call(this);
      else if (else_func)
        else_func.call(this);
    },
    _loop: function(first, last, func) {
      var items = this._items.slice(Math.min(first, this._items.length), Math.min(last, this._items.length));
      for(var i = 0; this._item = items[i]; i++)
        func.call(this);
      this._item = {};
    },

    _truncate: function(column, length) {
      var value = this._toString(this._value(column));
      if (value && length) {
        if (length <= 3) {
          this._print(value.length > length ? value.slice(0, length) : value);
        } else {
          this._print(value.length > length ? value.slice(0, length - 3) + '...' : value);
        }
      }
    },
    _comma: function(column) {
      var value = this._toString(this._value(column));
      while(value != (value = value.replace(/^(-?\d+)(\d{3})/, "$1,$2")));
      this._print(value);
    },
    _raw: function(column) {
      this._print(this._toRawString(this._value(column)));
    },

    _value: function(column) {
      if (column in this._item)
        return this._item[column];
      else if (column in this._globals)
        return this._globals[column];
      return null;
    },
    _script: function(content) {
      var renderer = this._clone();
      renderer._result = '';
      content.call(renderer);
      this._scripts.push(renderer._result);
    },
    _toString: function(value) {
      return this._toRawString(value).replace(/&/g, '&amp;').replace(/"/g, '&quot;').replace(/'/g, '&#39;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    },
    _toRawString: function(value) {
      if (typeof(value) == 'number')
        return String(value);
      else
        return !!value ? String(value) : '';
    },
    _toBoolean: function(value) {
      return this._toRawString(value) != '';
    },
    _equals: function(v1, v2) {
      return this._toRawString(v1) == this._toRawString(v2);
    }
  };

  if (!window._rcmdjp_disabled) {
    _rcmdjp._observeLoad(function() { _rcmdjp._trigger(); });
  }

  // for old version
  _rcmdjp._getItemRecommend = function(code) { return new _rcmdjp._ItemRecommend(code); };
  _rcmdjp._getItemRecommendByURL = function() { return new _rcmdjp._ItemRecommend(_rcmdjp._getItemCodeByURL()); };
  _rcmdjp._ItemRecommend = function(code) {
    this._code = code;
    this._data = [];
  };
  _rcmdjp._ItemRecommend.prototype = {
    _setData: function(key, value) { this._data.push([key, value]); },
    _track: function() {
      this._sendItemData();
      _rcmdjp._trackRecommend({ code: this._code });
    },
    _display: function(template, filter) {
      this._sendItemData();
      _rcmdjp._displayRecommend({ code: this._code, template: template, filter: filter });
    },
    _sendItemData: function() {
      if (this._data.length > 0) {
        var data = { code: this._code };
        for (var i = 0, pair; pair = this._data[i]; i++) data[pair[0]] = pair[1];
        _rcmdjp._setItemData(data);
        this._data = [];
      }
    }
  };
}
