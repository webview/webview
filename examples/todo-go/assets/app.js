'use strict';

var elementOpen = IncrementalDOM.elementOpen;
var elementClose = IncrementalDOM.elementClose;
var elementVoid = IncrementalDOM.elementVoid;
var text = IncrementalDOM.text;
var patch = IncrementalDOM.patch;

function renderDOM(items) {
  elementOpen('div', '', ['class', 'container']);
  elementOpen('form', '', ['class', 'text-input-wrapper'], 'onsubmit', function(e) {
  	var el = document.getElementById('task-name-input');
	rpc.addTask(el.value);
	el.value = '';
  });
  elementVoid('input', 'id-add', ['type', 'text', 'class', 'text-input', 'id', 'task-name-input']);
  for (var i = 0; i < items.length; i++) {
    (function(i) {
      elementOpen('div');
      var el =
	  elementVoid('input', i, ['type', 'checkbox', 'class', 'checkbox'],
		      'checked', items[i].done || undefined, 'onchange',
		      function(e) { rpc.markTask(i, e.target.checked); });
      el.checked = items[i].done;
      text(items[i].name);
      elementClose();
    })(i);
  }
  elementVoid('input', 'id-clear', ['type', 'button', 'class', 'btn clear-tasks',
	      'value', 'Clear completed tasks'], 'onclick',
	      function() { 
	      rpc.clearDoneTasks(); });
  elementClose();
  elementClose();
}

var rpc = {
  invoke : function(arg) { 
  window.external.invoke_(JSON.stringify(arg)); },
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
