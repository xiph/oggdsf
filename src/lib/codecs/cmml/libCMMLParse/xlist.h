//===========================================================================
//Copyright (C) 2005 Commonwealth Scientific and Industrial Research
//                   Organisation (CSIRO) Australia
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================


#ifndef __XLIST__
#define __XLIST__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * A doubly linked list
 */
typedef struct _XList XList;

struct _XList {
  XList * prev;
  XList * next;
  void * data;
};

/**
 * Signature of a cloning function.
 */
typedef void * (*XCloneFunc) (void * data);

/**
 * Signature of a freeing function.
 */
typedef void * (*XFreeFunc) (void * data);

/** Create a new list
 * \return a new list
 */
XList * xlist_new (void);

/**
 * Clone a list using the default clone function
 * \param list the list to clone
 * \returns a newly cloned list
 */
XList * xlist_clone (XList * list);

/**
 * Clone a list using a custom clone function
 * \param list the list to clone
 * \param clone the function to use to clone a list item
 * \returns a newly cloned list
 */
XList * xlist_clone_with (XList * list, XCloneFunc clone);

/**
 * Return the tail element of a list
 * \param list the list
 * \returns the tail element
 */
XList * xlist_tail (XList * list);

/**
 * Prepend a new node to a list containing given data
 * \param list the list
 * \param data the data element of the newly created node
 * \returns the new list head
 */
XList * xlist_prepend (XList * list, void * data);

/**
 * Append a new node to a list containing given data
 * \param list the list
 * \param data the data element of the newly created node
 * \returns the head of the list
 */
XList * xlist_append (XList * list, void * data);

/**
 * Add a new node containing given data before a given node
 * \param list the list
 * \param data the data element of the newly created node
 * \param node the node before which to add the newly created node
 * \returns the head of the list (which may have changed)
 */
XList * xlist_add_before (XList * list, void * data, XList * node);

/**
 * Add a new node containing given data after a given node
 * \param list the list
 * \param data the data element of the newly created node
 * \param node the node after which to add the newly created node
 * \returns the head of the list
 */
XList * xlist_add_after (XList * list, void * data, XList * node);

/**
 * Find the first node containing given data in a list
 * \param list the list
 * \param data the data element to find
 * \returns the first node containing given data, or NULL if it is not found
 */
XList * xlist_find (XList * list, void * data);

/**
 * Remove a node from a list
 * \param list the list
 * \param node the node to remove
 * \returns the head of the list (which may have changed)
 */
XList * xlist_remove (XList * list, XList * node);

/**
 * Query the number of items in a list
 * \param list the list
 * \returns the number of nodes in the list
 */
int xlist_length (XList * list);

/**
 * Query if a list is empty, ie. contains no items
 * \param list the list
 * \returns 1 if the list is empty, 0 otherwise
 */
int xlist_is_empty (XList * list);

/**
 * Query if the list is singleton, ie. contains exactly one item
 * \param list the list
 * \returns 1 if the list is singleton, 0 otherwise
 */
int xlist_is_singleton (XList * list);

/**
 * Free a list, using a given function to free each data element
 * \param list the list
 * \param free_func a function to free each data element
 * \returns NULL on success
 */
XList * xlist_free_with (XList * list, XFreeFunc free_func);

/**
 * Free a list, using anx_free() to free each data element
 * \param list the list
 * \returns NULL on success
 */
XList * xlist_free (XList * list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XLIST__ */


