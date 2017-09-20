'use strict';

var elementOpen = IncrementalDOM.elementOpen;
var elementClose = IncrementalDOM.elementClose;
var elementVoid = IncrementalDOM.elementVoid;
var text = IncrementalDOM.text;
var patch = IncrementalDOM.patch;

function renderDOM(items) {
  elementOpen('div', '', [ 'class', 'container' ]);
  elementOpen('form', '', [ 'class', 'text-input-wrapper' ], 'onsubmit',
	      function(e) {
		var el = document.getElementById('task-name-input');
		rpc.addTask(el.value);
		el.value = '';
	      });
  elementVoid('input', '', [
    'type', 'text', 'class', 'text-input', 'id', 'task-name-input', 'autofocus',
    'true'
  ]);
  elementOpen('div', '', [ 'class', 'task-list' ]);
  for (var i = 0; i < items.length; i++) {
    (function(i) {
      var checkedClass = (items[i].done ? 'checked' : 'unchecked');
      elementOpen('div', '', null, 'class', 'task-item ' + checkedClass,
		  'onclick', function() { rpc.markTask(i, !items[i].done); });
      text(items[i].name);
      elementClose();
    })(i);
  }
  elementClose();
  elementOpen('div', '', [ 'class', 'footer' ]);
  elementOpen('div', '', [ 'class', 'btn-clear-tasks' ], 'onclick',
	      function() { rpc.clearDoneTasks(); });
  text('Delete completed');
  elementClose();
  elementClose();
  elementClose();
  elementClose();
}

var rpc = {
  invoke : function(arg) { window.external.invoke_(JSON.stringify(arg)); },
  init : function() { rpc.invoke({cmd : 'init'}); },
  log : function() {
    var s = '';
    for (var i = 0; i < arguments.length; i++) {
      if (i != 0) {
	s = s + ' ';
      }
      s = s + JSON.stringify(arguments[i]);
    }
    rpc.invoke({cmd : 'log', text : s});
  },
  addTask : function(name) { rpc.invoke({cmd : 'addTask', name : name}); },
  clearDoneTasks : function() { rpc.invoke({cmd : 'clearDoneTasks'}); },
  markTask : function(index, done) {
    rpc.invoke({cmd : 'markTask', index : index, done : done});
  },
  render : function(items) {
    patch(document.body, function() { renderDOM(items); });
  }
};

window.onload = function() { rpc.init(); };
