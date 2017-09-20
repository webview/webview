
/**
 * @license
 * Copyright 2015 The Incremental DOM Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS-IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

(function (global, factory) {
  typeof exports === 'object' && typeof module !== 'undefined' ? factory(exports) :
  typeof define === 'function' && define.amd ? define(['exports'], factory) :
  (factory((global.IncrementalDOM = {})));
}(this, function (exports) { 'use strict';

  /**
   * Copyright 2015 The Incremental DOM Authors. All Rights Reserved.
   *
   * Licensed under the Apache License, Version 2.0 (the "License");
   * you may not use this file except in compliance with the License.
   * You may obtain a copy of the License at
   *
   *      http://www.apache.org/licenses/LICENSE-2.0
   *
   * Unless required by applicable law or agreed to in writing, software
   * distributed under the License is distributed on an "AS-IS" BASIS,
   * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   * See the License for the specific language governing permissions and
   * limitations under the License.
   */

  /**
   * A cached reference to the hasOwnProperty function.
   */
  var hasOwnProperty = Object.prototype.hasOwnProperty;

  /**
   * A constructor function that will create blank objects.
   * @constructor
   */
  function Blank() {}

  Blank.prototype = Object.create(null);

  /**
   * Used to prevent property collisions between our "map" and its prototype.
   * @param {!Object<string, *>} map The map to check.
   * @param {string} property The property to check.
   * @return {boolean} Whether map has property.
   */
  var has = function (map, property) {
    return hasOwnProperty.call(map, property);
  };

  /**
   * Creates an map object without a prototype.
   * @return {!Object}
   */
  var createMap = function () {
    return new Blank();
  };

  /**
   * The property name where we store Incremental DOM data.
   */
  var DATA_PROP = '__incrementalDOMData';

  /**
   * Keeps track of information needed to perform diffs for a given DOM node.
   * @param {!string} nodeName
   * @param {?string=} key
   * @constructor
   */
  function NodeData(nodeName, key) {
    /**
     * The attributes and their values.
     * @const {!Object<string, *>}
     */
    this.attrs = createMap();

    /**
     * An array of attribute name/value pairs, used for quickly diffing the
     * incomming attributes to see if the DOM node's attributes need to be
     * updated.
     * @const {Array<*>}
     */
    this.attrsArr = [];

    /**
     * The incoming attributes for this Node, before they are updated.
     * @const {!Object<string, *>}
     */
    this.newAttrs = createMap();

    /**
     * Whether or not the statics have been applied for the node yet.
     * {boolean}
     */
    this.staticsApplied = false;

    /**
     * The key used to identify this node, used to preserve DOM nodes when they
     * move within their parent.
     * @const
     */
    this.key = key;

    /**
     * Keeps track of children within this node by their key.
     * {!Object<string, !Element>}
     */
    this.keyMap = createMap();

    /**
     * Whether or not the keyMap is currently valid.
     * @type {boolean}
     */
    this.keyMapValid = true;

    /**
     * Whether or the associated node is, or contains, a focused Element.
     * @type {boolean}
     */
    this.focused = false;

    /**
     * The node name for this node.
     * @const {string}
     */
    this.nodeName = nodeName;

    /**
     * @type {?string}
     */
    this.text = null;
  }

  /**
   * Initializes a NodeData object for a Node.
   *
   * @param {Node} node The node to initialize data for.
   * @param {string} nodeName The node name of node.
   * @param {?string=} key The key that identifies the node.
   * @return {!NodeData} The newly initialized data object
   */
  var initData = function (node, nodeName, key) {
    var data = new NodeData(nodeName, key);
    node[DATA_PROP] = data;
    return data;
  };

  /**
   * Retrieves the NodeData object for a Node, creating it if necessary.
   *
   * @param {?Node} node The Node to retrieve the data for.
   * @return {!NodeData} The NodeData for this Node.
   */
  var getData = function (node) {
    importNode(node);
    return node[DATA_PROP];
  };

  /**
   * Imports node and its subtree, initializing caches.
   *
   * @param {?Node} node The Node to import.
   */
  var importNode = function (node) {
    if (node[DATA_PROP]) {
      return;
    }

    var isElement = node instanceof Element;
    var nodeName = isElement ? node.localName : node.nodeName;
    var key = isElement ? node.getAttribute('key') : null;
    var data = initData(node, nodeName, key);

    if (key) {
      getData(node.parentNode).keyMap[key] = node;
    }

    if (isElement) {
      var attributes = node.attributes;
      var attrs = data.attrs;
      var newAttrs = data.newAttrs;
      var attrsArr = data.attrsArr;

      for (var i = 0; i < attributes.length; i += 1) {
        var attr = attributes[i];
        var name = attr.name;
        var value = attr.value;

        attrs[name] = value;
        newAttrs[name] = undefined;
        attrsArr.push(name);
        attrsArr.push(value);
      }
    }

    for (var child = node.firstChild; child; child = child.nextSibling) {
      importNode(child);
    }
  };

  /**
   * Gets the namespace to create an element (of a given tag) in.
   * @param {string} tag The tag to get the namespace for.
   * @param {?Node} parent
   * @return {?string} The namespace to create the tag in.
   */
  var getNamespaceForTag = function (tag, parent) {
    if (tag === 'svg') {
      return 'http://www.w3.org/2000/svg';
    }

    if (getData(parent).nodeName === 'foreignObject') {
      return null;
    }

    return parent.namespaceURI;
  };

  /**
   * Creates an Element.
   * @param {Document} doc The document with which to create the Element.
   * @param {?Node} parent
   * @param {string} tag The tag for the Element.
   * @param {?string=} key A key to identify the Element.
   * @return {!Element}
   */
  var createElement = function (doc, parent, tag, key) {
    var namespace = getNamespaceForTag(tag, parent);
    var el = undefined;

    if (namespace) {
      el = doc.createElementNS(namespace, tag);
    } else {
      el = doc.createElement(tag);
    }

    initData(el, tag, key);

    return el;
  };

  /**
   * Creates a Text Node.
   * @param {Document} doc The document with which to create the Element.
   * @return {!Text}
   */
  var createText = function (doc) {
    var node = doc.createTextNode('');
    initData(node, '#text', null);
    return node;
  };

  /**
   * Copyright 2015 The Incremental DOM Authors. All Rights Reserved.
   *
   * Licensed under the Apache License, Version 2.0 (the "License");
   * you may not use this file except in compliance with the License.
   * You may obtain a copy of the License at
   *
   *      http://www.apache.org/licenses/LICENSE-2.0
   *
   * Unless required by applicable law or agreed to in writing, software
   * distributed under the License is distributed on an "AS-IS" BASIS,
   * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   * See the License for the specific language governing permissions and
   * limitations under the License.
   */

  /** @const */
  var notifications = {
    /**
     * Called after patch has compleated with any Nodes that have been created
     * and added to the DOM.
     * @type {?function(Array<!Node>)}
     */
    nodesCreated: null,

    /**
     * Called after patch has compleated with any Nodes that have been removed
     * from the DOM.
     * Note it's an applications responsibility to handle any childNodes.
     * @type {?function(Array<!Node>)}
     */
    nodesDeleted: null
  };

  /**
   * Keeps track of the state of a patch.
   * @constructor
   */
  function Context() {
    /**
     * @type {(Array<!Node>|undefined)}
     */
    this.created = notifications.nodesCreated && [];

    /**
     * @type {(Array<!Node>|undefined)}
     */
    this.deleted = notifications.nodesDeleted && [];
  }

  /**
   * @param {!Node} node
   */
  Context.prototype.markCreated = function (node) {
    if (this.created) {
      this.created.push(node);
    }
  };

  /**
   * @param {!Node} node
   */
  Context.prototype.markDeleted = function (node) {
    if (this.deleted) {
      this.deleted.push(node);
    }
  };

  /**
   * Notifies about nodes that were created during the patch opearation.
   */
  Context.prototype.notifyChanges = function () {
    if (this.created && this.created.length > 0) {
      notifications.nodesCreated(this.created);
    }

    if (this.deleted && this.deleted.length > 0) {
      notifications.nodesDeleted(this.deleted);
    }
  };

  /**
   * Copyright 2016 The Incremental DOM Authors. All Rights Reserved.
   *
   * Licensed under the Apache License, Version 2.0 (the "License");
   * you may not use this file except in compliance with the License.
   * You may obtain a copy of the License at
   *
   *      http://www.apache.org/licenses/LICENSE-2.0
   *
   * Unless required by applicable law or agreed to in writing, software
   * distributed under the License is distributed on an "AS-IS" BASIS,
   * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   * See the License for the specific language governing permissions and
   * limitations under the License.
   */

  /**
   * @param {!Node} node
   * @return {boolean} True if the node the root of a document, false otherwise.
   */
  var isDocumentRoot = function (node) {
    // For ShadowRoots, check if they are a DocumentFragment instead of if they
    // are a ShadowRoot so that this can work in 'use strict' if ShadowRoots are
    // not supported.
    return node instanceof Document || node instanceof DocumentFragment;
  };

  /**
   * @param {!Node} node The node to start at, inclusive.
   * @param {?Node} root The root ancestor to get until, exclusive.
   * @return {!Array<!Node>} The ancestry of DOM nodes.
   */
  var getAncestry = function (node, root) {
    var ancestry = [];
    var cur = node;

    while (cur !== root) {
      ancestry.push(cur);
      cur = cur.parentNode;
    }

    return ancestry;
  };

  /**
   * @param {!Node} node
   * @return {!Node} The root node of the DOM tree that contains node.
   */
  var getRoot = function (node) {
    var cur = node;
    var prev = cur;

    while (cur) {
      prev = cur;
      cur = cur.parentNode;
    }

    return prev;
  };

  /**
   * @param {!Node} node The node to get the activeElement for.
   * @return {?Element} The activeElement in the Document or ShadowRoot
   *     corresponding to node, if present.
   */
  var getActiveElement = function (node) {
    var root = getRoot(node);
    return isDocumentRoot(root) ? root.activeElement : null;
  };

  /**
   * Gets the path of nodes that contain the focused node in the same document as
   * a reference node, up until the root.
   * @param {!Node} node The reference node to get the activeElement for.
   * @param {?Node} root The root to get the focused path until.
   * @return {!Array<Node>}
   */
  var getFocusedPath = function (node, root) {
    var activeElement = getActiveElement(node);

    if (!activeElement || !node.contains(activeElement)) {
      return [];
    }

    return getAncestry(activeElement, root);
  };

  /**
   * Like insertBefore, but instead instead of moving the desired node, instead
   * moves all the other nodes after.
   * @param {?Node} parentNode
   * @param {!Node} node
   * @param {?Node} referenceNode
   */
  var moveBefore = function (parentNode, node, referenceNode) {
    var insertReferenceNode = node.nextSibling;
    var cur = referenceNode;

    while (cur !== node) {
      var next = cur.nextSibling;
      parentNode.insertBefore(cur, insertReferenceNode);
      cur = next;
    }
  };

  /** @type {?Context} */
  var context = null;

  /** @type {?Node} */
  var currentNode = null;

  /** @type {?Node} */
  var currentParent = null;

  /** @type {?Document} */
  var doc = null;

  /**
   * @param {!Array<Node>} focusPath The nodes to mark.
   * @param {boolean} focused Whether or not they are focused.
   */
  var markFocused = function (focusPath, focused) {
    for (var i = 0; i < focusPath.length; i += 1) {
      getData(focusPath[i]).focused = focused;
    }
  };

  /**
   * Returns a patcher function that sets up and restores a patch context,
   * running the run function with the provided data.
   * @param {function((!Element|!DocumentFragment),!function(T),T=): ?Node} run
   * @return {function((!Element|!DocumentFragment),!function(T),T=): ?Node}
   * @template T
   */
  var patchFactory = function (run) {
    /**
     * TODO(moz): These annotations won't be necessary once we switch to Closure
     * Compiler's new type inference. Remove these once the switch is done.
     *
     * @param {(!Element|!DocumentFragment)} node
     * @param {!function(T)} fn
     * @param {T=} data
     * @return {?Node} node
     * @template T
     */
    var f = function (node, fn, data) {
      var prevContext = context;
      var prevDoc = doc;
      var prevCurrentNode = currentNode;
      var prevCurrentParent = currentParent;
      var previousInAttributes = false;
      var previousInSkip = false;

      context = new Context();
      doc = node.ownerDocument;
      currentParent = node.parentNode;

      if ('production' !== 'production') {}

      var focusPath = getFocusedPath(node, currentParent);
      markFocused(focusPath, true);
      var retVal = run(node, fn, data);
      markFocused(focusPath, false);

      if ('production' !== 'production') {}

      context.notifyChanges();

      context = prevContext;
      doc = prevDoc;
      currentNode = prevCurrentNode;
      currentParent = prevCurrentParent;

      return retVal;
    };
    return f;
  };

  /**
   * Patches the document starting at node with the provided function. This
   * function may be called during an existing patch operation.
   * @param {!Element|!DocumentFragment} node The Element or Document
   *     to patch.
   * @param {!function(T)} fn A function containing elementOpen/elementClose/etc.
   *     calls that describe the DOM.
   * @param {T=} data An argument passed to fn to represent DOM state.
   * @return {!Node} The patched node.
   * @template T
   */
  var patchInner = patchFactory(function (node, fn, data) {
    currentNode = node;

    enterNode();
    fn(data);
    exitNode();

    if ('production' !== 'production') {}

    return node;
  });

  /**
   * Patches an Element with the the provided function. Exactly one top level
   * element call should be made corresponding to `node`.
   * @param {!Element} node The Element where the patch should start.
   * @param {!function(T)} fn A function containing elementOpen/elementClose/etc.
   *     calls that describe the DOM. This should have at most one top level
   *     element call.
   * @param {T=} data An argument passed to fn to represent DOM state.
   * @return {?Node} The node if it was updated, its replacedment or null if it
   *     was removed.
   * @template T
   */
  var patchOuter = patchFactory(function (node, fn, data) {
    var startNode = /** @type {!Element} */{ nextSibling: node };
    var expectedNextNode = null;
    var expectedPrevNode = null;

    if ('production' !== 'production') {}

    currentNode = startNode;
    fn(data);

    if ('production' !== 'production') {}

    if (node !== currentNode && node.parentNode) {
      removeChild(currentParent, node, getData(currentParent).keyMap);
    }

    return startNode === currentNode ? null : currentNode;
  });

  /**
   * Checks whether or not the current node matches the specified nodeName and
   * key.
   *
   * @param {!Node} matchNode A node to match the data to.
   * @param {?string} nodeName The nodeName for this node.
   * @param {?string=} key An optional key that identifies a node.
   * @return {boolean} True if the node matches, false otherwise.
   */
  var matches = function (matchNode, nodeName, key) {
    var data = getData(matchNode);

    // Key check is done using double equals as we want to treat a null key the
    // same as undefined. This should be okay as the only values allowed are
    // strings, null and undefined so the == semantics are not too weird.
    return nodeName === data.nodeName && key == data.key;
  };

  /**
   * Aligns the virtual Element definition with the actual DOM, moving the
   * corresponding DOM node to the correct location or creating it if necessary.
   * @param {string} nodeName For an Element, this should be a valid tag string.
   *     For a Text, this should be #text.
   * @param {?string=} key The key used to identify this element.
   */
  var alignWithDOM = function (nodeName, key) {
    if (currentNode && matches(currentNode, nodeName, key)) {
      return;
    }

    var parentData = getData(currentParent);
    var currentNodeData = currentNode && getData(currentNode);
    var keyMap = parentData.keyMap;
    var node = undefined;

    // Check to see if the node has moved within the parent.
    if (key) {
      var keyNode = keyMap[key];
      if (keyNode) {
        if (matches(keyNode, nodeName, key)) {
          node = keyNode;
        } else if (keyNode === currentNode) {
          context.markDeleted(keyNode);
        } else {
          removeChild(currentParent, keyNode, keyMap);
        }
      }
    }

    // Create the node if it doesn't exist.
    if (!node) {
      if (nodeName === '#text') {
        node = createText(doc);
      } else {
        node = createElement(doc, currentParent, nodeName, key);
      }

      if (key) {
        keyMap[key] = node;
      }

      context.markCreated(node);
    }

    // Re-order the node into the right position, preserving focus if either
    // node or currentNode are focused by making sure that they are not detached
    // from the DOM.
    if (getData(node).focused) {
      // Move everything else before the node.
      moveBefore(currentParent, node, currentNode);
    } else if (currentNodeData && currentNodeData.key && !currentNodeData.focused) {
      // Remove the currentNode, which can always be added back since we hold a
      // reference through the keyMap. This prevents a large number of moves when
      // a keyed item is removed or moved backwards in the DOM.
      currentParent.replaceChild(node, currentNode);
      parentData.keyMapValid = false;
    } else {
      currentParent.insertBefore(node, currentNode);
    }

    currentNode = node;
  };

  /**
   * @param {?Node} node
   * @param {?Node} child
   * @param {?Object<string, !Element>} keyMap
   */
  var removeChild = function (node, child, keyMap) {
    node.removeChild(child);
    context.markDeleted( /** @type {!Node}*/child);

    var key = getData(child).key;
    if (key) {
      delete keyMap[key];
    }
  };

  /**
   * Clears out any unvisited Nodes, as the corresponding virtual element
   * functions were never called for them.
   */
  var clearUnvisitedDOM = function () {
    var node = currentParent;
    var data = getData(node);
    var keyMap = data.keyMap;
    var keyMapValid = data.keyMapValid;
    var child = node.lastChild;
    var key = undefined;

    if (child === currentNode && keyMapValid) {
      return;
    }

    while (child !== currentNode) {
      removeChild(node, child, keyMap);
      child = node.lastChild;
    }

    // Clean the keyMap, removing any unusued keys.
    if (!keyMapValid) {
      for (key in keyMap) {
        child = keyMap[key];
        if (child.parentNode !== node) {
          context.markDeleted(child);
          delete keyMap[key];
        }
      }

      data.keyMapValid = true;
    }
  };

  /**
   * Changes to the first child of the current node.
   */
  var enterNode = function () {
    currentParent = currentNode;
    currentNode = null;
  };

  /**
   * @return {?Node} The next Node to be patched.
   */
  var getNextNode = function () {
    if (currentNode) {
      return currentNode.nextSibling;
    } else {
      return currentParent.firstChild;
    }
  };

  /**
   * Changes to the next sibling of the current node.
   */
  var nextNode = function () {
    currentNode = getNextNode();
  };

  /**
   * Changes to the parent of the current node, removing any unvisited children.
   */
  var exitNode = function () {
    clearUnvisitedDOM();

    currentNode = currentParent;
    currentParent = currentParent.parentNode;
  };

  /**
   * Makes sure that the current node is an Element with a matching tagName and
   * key.
   *
   * @param {string} tag The element's tag.
   * @param {?string=} key The key used to identify this element. This can be an
   *     empty string, but performance may be better if a unique value is used
   *     when iterating over an array of items.
   * @return {!Element} The corresponding Element.
   */
  var coreElementOpen = function (tag, key) {
    nextNode();
    alignWithDOM(tag, key);
    enterNode();
    return (/** @type {!Element} */currentParent
    );
  };

  /**
   * Closes the currently open Element, removing any unvisited children if
   * necessary.
   *
   * @return {!Element} The corresponding Element.
   */
  var coreElementClose = function () {
    if ('production' !== 'production') {}

    exitNode();
    return (/** @type {!Element} */currentNode
    );
  };

  /**
   * Makes sure the current node is a Text node and creates a Text node if it is
   * not.
   *
   * @return {!Text} The corresponding Text Node.
   */
  var coreText = function () {
    nextNode();
    alignWithDOM('#text', null);
    return (/** @type {!Text} */currentNode
    );
  };

  /**
   * Gets the current Element being patched.
   * @return {!Element}
   */
  var currentElement = function () {
    if ('production' !== 'production') {}
    return (/** @type {!Element} */currentParent
    );
  };

  /**
   * @return {Node} The Node that will be evaluated for the next instruction.
   */
  var currentPointer = function () {
    if ('production' !== 'production') {}
    return getNextNode();
  };

  /**
   * Skips the children in a subtree, allowing an Element to be closed without
   * clearing out the children.
   */
  var skip = function () {
    if ('production' !== 'production') {}
    currentNode = currentParent.lastChild;
  };

  /**
   * Skips the next Node to be patched, moving the pointer forward to the next
   * sibling of the current pointer.
   */
  var skipNode = nextNode;

  /**
   * Copyright 2015 The Incremental DOM Authors. All Rights Reserved.
   *
   * Licensed under the Apache License, Version 2.0 (the "License");
   * you may not use this file except in compliance with the License.
   * You may obtain a copy of the License at
   *
   *      http://www.apache.org/licenses/LICENSE-2.0
   *
   * Unless required by applicable law or agreed to in writing, software
   * distributed under the License is distributed on an "AS-IS" BASIS,
   * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   * See the License for the specific language governing permissions and
   * limitations under the License.
   */

  /** @const */
  var symbols = {
    default: '__default'
  };

  /**
   * @param {string} name
   * @return {string|undefined} The namespace to use for the attribute.
   */
  var getNamespace = function (name) {
    if (name.lastIndexOf('xml:', 0) === 0) {
      return 'http://www.w3.org/XML/1998/namespace';
    }

    if (name.lastIndexOf('xlink:', 0) === 0) {
      return 'http://www.w3.org/1999/xlink';
    }
  };

  /**
   * Applies an attribute or property to a given Element. If the value is null
   * or undefined, it is removed from the Element. Otherwise, the value is set
   * as an attribute.
   * @param {!Element} el
   * @param {string} name The attribute's name.
   * @param {?(boolean|number|string)=} value The attribute's value.
   */
  var applyAttr = function (el, name, value) {
    if (value == null) {
      el.removeAttribute(name);
    } else {
      var attrNS = getNamespace(name);
      if (attrNS) {
        el.setAttributeNS(attrNS, name, value);
      } else {
        el.setAttribute(name, value);
      }
    }
  };

  /**
   * Applies a property to a given Element.
   * @param {!Element} el
   * @param {string} name The property's name.
   * @param {*} value The property's value.
   */
  var applyProp = function (el, name, value) {
    el[name] = value;
  };

  /**
   * Applies a value to a style declaration. Supports CSS custom properties by
   * setting properties containing a dash using CSSStyleDeclaration.setProperty.
   * @param {CSSStyleDeclaration} style
   * @param {!string} prop
   * @param {*} value
   */
  var setStyleValue = function (style, prop, value) {
    if (prop.indexOf('-') >= 0) {
      style.setProperty(prop, /** @type {string} */value);
    } else {
      style[prop] = value;
    }
  };

  /**
   * Applies a style to an Element. No vendor prefix expansion is done for
   * property names/values.
   * @param {!Element} el
   * @param {string} name The attribute's name.
   * @param {*} style The style to set. Either a string of css or an object
   *     containing property-value pairs.
   */
  var applyStyle = function (el, name, style) {
    if (typeof style === 'string') {
      el.style.cssText = style;
    } else {
      el.style.cssText = '';
      var elStyle = el.style;
      var obj = /** @type {!Object<string,string>} */style;

      for (var prop in obj) {
        if (has(obj, prop)) {
          setStyleValue(elStyle, prop, obj[prop]);
        }
      }
    }
  };

  /**
   * Updates a single attribute on an Element.
   * @param {!Element} el
   * @param {string} name The attribute's name.
   * @param {*} value The attribute's value. If the value is an object or
   *     function it is set on the Element, otherwise, it is set as an HTML
   *     attribute.
   */
  var applyAttributeTyped = function (el, name, value) {
    var type = typeof value;

    if (type === 'object' || type === 'function') {
      applyProp(el, name, value);
    } else {
      applyAttr(el, name, /** @type {?(boolean|number|string)} */value);
    }
  };

  /**
   * Calls the appropriate attribute mutator for this attribute.
   * @param {!Element} el
   * @param {string} name The attribute's name.
   * @param {*} value The attribute's value.
   */
  var updateAttribute = function (el, name, value) {
    var data = getData(el);
    var attrs = data.attrs;

    if (attrs[name] === value) {
      return;
    }

    var mutator = attributes[name] || attributes[symbols.default];
    mutator(el, name, value);

    attrs[name] = value;
  };

  /**
   * A publicly mutable object to provide custom mutators for attributes.
   * @const {!Object<string, function(!Element, string, *)>}
   */
  var attributes = createMap();

  // Special generic mutator that's called for any attribute that does not
  // have a specific mutator.
  attributes[symbols.default] = applyAttributeTyped;

  attributes['style'] = applyStyle;

  /**
   * The offset in the virtual element declaration where the attributes are
   * specified.
   * @const
   */
  var ATTRIBUTES_OFFSET = 3;

  /**
   * Builds an array of arguments for use with elementOpenStart, attr and
   * elementOpenEnd.
   * @const {Array<*>}
   */
  var argsBuilder = [];

  /**
   * @param {string} tag The element's tag.
   * @param {?string=} key The key used to identify this element. This can be an
   *     empty string, but performance may be better if a unique value is used
   *     when iterating over an array of items.
   * @param {?Array<*>=} statics An array of attribute name/value pairs of the
   *     static attributes for the Element. These will only be set once when the
   *     Element is created.
   * @param {...*} var_args, Attribute name/value pairs of the dynamic attributes
   *     for the Element.
   * @return {!Element} The corresponding Element.
   */
  var elementOpen = function (tag, key, statics, var_args) {
    if ('production' !== 'production') {}

    var node = coreElementOpen(tag, key);
    var data = getData(node);

    if (!data.staticsApplied) {
      if (statics) {
        for (var _i = 0; _i < statics.length; _i += 2) {
          var name = /** @type {string} */statics[_i];
          var value = statics[_i + 1];
          updateAttribute(node, name, value);
        }
      }
      // Down the road, we may want to keep track of the statics array to use it
      // as an additional signal about whether a node matches or not. For now,
      // just use a marker so that we do not reapply statics.
      data.staticsApplied = true;
    }

    /*
     * Checks to see if one or more attributes have changed for a given Element.
     * When no attributes have changed, this is much faster than checking each
     * individual argument. When attributes have changed, the overhead of this is
     * minimal.
     */
    var attrsArr = data.attrsArr;
    var newAttrs = data.newAttrs;
    var isNew = !attrsArr.length;
    var i = ATTRIBUTES_OFFSET;
    var j = 0;

    for (; i < arguments.length; i += 2, j += 2) {
      var _attr = arguments[i];
      if (isNew) {
        attrsArr[j] = _attr;
        newAttrs[_attr] = undefined;
      } else if (attrsArr[j] !== _attr) {
        break;
      }

      var value = arguments[i + 1];
      if (isNew || attrsArr[j + 1] !== value) {
        attrsArr[j + 1] = value;
        updateAttribute(node, _attr, value);
      }
    }

    if (i < arguments.length || j < attrsArr.length) {
      for (; i < arguments.length; i += 1, j += 1) {
        attrsArr[j] = arguments[i];
      }

      if (j < attrsArr.length) {
        attrsArr.length = j;
      }

      /*
       * Actually perform the attribute update.
       */
      for (i = 0; i < attrsArr.length; i += 2) {
        var name = /** @type {string} */attrsArr[i];
        var value = attrsArr[i + 1];
        newAttrs[name] = value;
      }

      for (var _attr2 in newAttrs) {
        updateAttribute(node, _attr2, newAttrs[_attr2]);
        newAttrs[_attr2] = undefined;
      }
    }

    return node;
  };

  /**
   * Declares a virtual Element at the current location in the document. This
   * corresponds to an opening tag and a elementClose tag is required. This is
   * like elementOpen, but the attributes are defined using the attr function
   * rather than being passed as arguments. Must be folllowed by 0 or more calls
   * to attr, then a call to elementOpenEnd.
   * @param {string} tag The element's tag.
   * @param {?string=} key The key used to identify this element. This can be an
   *     empty string, but performance may be better if a unique value is used
   *     when iterating over an array of items.
   * @param {?Array<*>=} statics An array of attribute name/value pairs of the
   *     static attributes for the Element. These will only be set once when the
   *     Element is created.
   */
  var elementOpenStart = function (tag, key, statics) {
    if ('production' !== 'production') {}

    argsBuilder[0] = tag;
    argsBuilder[1] = key;
    argsBuilder[2] = statics;
  };

  /***
   * Defines a virtual attribute at this point of the DOM. This is only valid
   * when called between elementOpenStart and elementOpenEnd.
   *
   * @param {string} name
   * @param {*} value
   */
  var attr = function (name, value) {
    if ('production' !== 'production') {}

    argsBuilder.push(name);
    argsBuilder.push(value);
  };

  /**
   * Closes an open tag started with elementOpenStart.
   * @return {!Element} The corresponding Element.
   */
  var elementOpenEnd = function () {
    if ('production' !== 'production') {}

    var node = elementOpen.apply(null, argsBuilder);
    argsBuilder.length = 0;
    return node;
  };

  /**
   * Closes an open virtual Element.
   *
   * @param {string} tag The element's tag.
   * @return {!Element} The corresponding Element.
   */
  var elementClose = function (tag) {
    if ('production' !== 'production') {}

    var node = coreElementClose();

    if ('production' !== 'production') {}

    return node;
  };

  /**
   * Declares a virtual Element at the current location in the document that has
   * no children.
   * @param {string} tag The element's tag.
   * @param {?string=} key The key used to identify this element. This can be an
   *     empty string, but performance may be better if a unique value is used
   *     when iterating over an array of items.
   * @param {?Array<*>=} statics An array of attribute name/value pairs of the
   *     static attributes for the Element. These will only be set once when the
   *     Element is created.
   * @param {...*} var_args Attribute name/value pairs of the dynamic attributes
   *     for the Element.
   * @return {!Element} The corresponding Element.
   */
  var elementVoid = function (tag, key, statics, var_args) {
    elementOpen.apply(null, arguments);
    return elementClose(tag);
  };

  /**
   * Declares a virtual Text at this point in the document.
   *
   * @param {string|number|boolean} value The value of the Text.
   * @param {...(function((string|number|boolean)):string)} var_args
   *     Functions to format the value which are called only when the value has
   *     changed.
   * @return {!Text} The corresponding text node.
   */
  var text = function (value, var_args) {
    if ('production' !== 'production') {}

    var node = coreText();
    var data = getData(node);

    if (data.text !== value) {
      data.text = /** @type {string} */value;

      var formatted = value;
      for (var i = 1; i < arguments.length; i += 1) {
        /*
         * Call the formatter function directly to prevent leaking arguments.
         * https://github.com/google/incremental-dom/pull/204#issuecomment-178223574
         */
        var fn = arguments[i];
        formatted = fn(formatted);
      }

      node.data = formatted;
    }

    return node;
  };

  exports.patch = patchInner;
  exports.patchInner = patchInner;
  exports.patchOuter = patchOuter;
  exports.currentElement = currentElement;
  exports.currentPointer = currentPointer;
  exports.skip = skip;
  exports.skipNode = skipNode;
  exports.elementVoid = elementVoid;
  exports.elementOpenStart = elementOpenStart;
  exports.elementOpenEnd = elementOpenEnd;
  exports.elementOpen = elementOpen;
  exports.elementClose = elementClose;
  exports.text = text;
  exports.attr = attr;
  exports.symbols = symbols;
  exports.attributes = attributes;
  exports.applyAttr = applyAttr;
  exports.applyProp = applyProp;
  exports.notifications = notifications;
  exports.importNode = importNode;

}));

//# sourceMappingURL=incremental-dom.js.map