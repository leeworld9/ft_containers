/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   avl_tree.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dohelee <dohelee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/06 15:43:58 by dohelee           #+#    #+#             */
/*   Updated: 2021/12/07 15:30:08 by dohelee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AVL_TREE_HPP
# define AVL_TREE_HPP

#include <iostream>
#include "./avl_tree_iterator.hpp"
#include "./utility.hpp"
#include "./Node.hpp"

namespace ft
{
    //reference1 : https://yoongrammer.tistory.com/72
    //reference2 : https://doublesprogramming.tistory.com/237

    template <class T, class Alloc, class Compare,
			  class Node = ft::Node<T> >
	class avl_tree
	{
		public :
			typedef T   value_type;
            typedef value_type& reference;
            typedef value_type const& const_reference;
            typedef value_type* pointer;
            typedef value_type const* const_pointer;
			typedef Node node_type;
            typedef node_type& node_reference;
            typedef node_type const& const_node_reference;
            typedef node_type* node_pointer;
            typedef node_type const* const_node_pointer;
			typedef typename Alloc::template rebind<node_type>::other  node_alloc; // rebind
            typedef Compare key_compare;
			typedef typename ft::avl_tree_iterator<value_type, node_type, key_compare> iterator;
			typedef typename ft::avl_tree_const_iterator<const value_type, node_type, key_compare> const_iterator;
			typedef size_t size_type;

			avl_tree (const node_alloc& alloc = node_alloc(), const key_compare& comp = key_compare())
			: alloc(alloc), comp(comp)
			{
				this->insert_node = NULL;
                this->exists_node = NULL;
                this->super_root = this->alloc.allocate(1);
				this->alloc.construct(this->super_root, Node());
                (this->super_root)->parent = NULL;
                this->parent = this->super_root;
			}

            avl_tree(const avl_tree& copy)
            {
                *this = copy;
            }

		    avl_tree& operator= (const avl_tree& at)
            {
                if (this != &at)
				{
                    this->insert_node = at.insert_node;
                    this->exists_node = at.exists_node;
                    this->alloc = at.alloc;
                    this->super_root = at.super_root;
                    this->parent = at.parent;
                    this->comp = comp;
                }
				return (*this);
            }

			virtual ~avl_tree () { }

            int max_height(node_pointer a, node_pointer b)
            {
                if (a == NULL && b == NULL)
                    return 0;
                else if (a == NULL && b != NULL)
                    return b->height;
                else if (a != NULL && b == NULL)
                    return a->height;
                else
                    return (a->height > b->height) ? a->height : b->height;
            }

            node_pointer new_node(value_type key)
            {
                node_pointer nw = this->alloc.allocate(1);
				this->alloc.construct(nw, Node(key));
                nw->parent = this->parent;
                if (nw->parent == this->super_root) // super_root->left == root
                    (this->super_root)->left = nw;
                this->parent = NULL; // ?????? ?????? ??????
                this->insert_node = nw; // ?????? ????????? ?????? ??????
                return nw;
            }

            node_pointer leftRotate (node_pointer z)
            {
                node_pointer y = z->right;
                node_pointer T2 = y->left;

                // left ?????? ??????
                y->left = z;
                z->right = T2;

                // parent ?????? ??????
                y->parent = z->parent;
                z->parent = y;
                if (T2 != NULL)
                    T2->parent = z;

                // ?????? ??? y??? root_node??? ?????? ?????? super_root->left ?????? ??????
                if (y->parent == this->super_root)
                    (this->super_root)->left = y;

                // ?????? ?????? ??????
                z->height = 1 + max_height(z->left, z->right);
                y->height = 1 + max_height(y->left, y->right);

                // ????????? ?????? ?????? y??? ??????  
                return y;
            }

            node_pointer rightRotate (node_pointer z)
            {
                node_pointer y = z->left;
                node_pointer T2 = y->right;

                // right ?????? ??????
                y->right = z;
                z->left = T2;

                // parent ?????? ??????
                y->parent = z->parent;
                z->parent = y;
                if (T2 != NULL)
                    T2->parent = z;

                // ?????? ??? y??? root_node??? ?????? ?????? super_root->left ?????? ??????
                if (y->parent == this->super_root)
                    (this->super_root)->left = y;

                // ?????? ?????? ??????
                z->height = 1 + max_height(z->left, z->right);
                y->height = 1 + max_height(y->left, y->right);

                // ????????? ?????? ?????? y??? ??????  
                return y;
            }

            // BF(BalanceFactor)?????? ???????????? ??????.
            int getBalanceFactor(node_pointer n)
            {
                if (n == NULL)
                    return 0;
                else
                {
                    int left_height = (n->left != NULL) ? n->left->height: 0;
                    int right_height = (n->right != NULL) ? n->right->height : 0;
                    return left_height - right_height;
                }
            }

            // ????????? ?????? ????????? ???????????? ?????? (insert)
            // 4?????? ???????????? ????????? rotate??? ?????????.
            node_pointer insert_rebalance(node_pointer node, value_type key)
            {
                int bFactor = getBalanceFactor(node);

                // LL Case
                if (bFactor > 1 && comp(key.first, node->left->value.first)) // bFactor > 1 && key.first < node->left->value.first
                    return rightRotate(node);
                // RR Case
                if (bFactor < -1 && comp(node->right->value.first, key.first)) // bFactor < -1 && key.first > node->right->value.first
                    return leftRotate(node);
                // LR Case
                if (bFactor > 1 && comp(node->left->value.first, key.first)) // bFactor > 1 && key.first > node->left->value.first
                {
                    node->left = leftRotate(node->left);
                    return rightRotate(node);
                }
                // RL Case
                if (bFactor < -1 && comp(key.first, node->right->value.first)) // bFactor < -1 && key.first < node->right->value.first
                {
                    node->right = rightRotate(node->right);
                    return leftRotate(node);
                }

                return node;
            }

            // ?????? ??????.
            node_pointer insert(node_pointer root, value_type key)
            {
                // ?????? ??????
                if (root == NULL)
                    return new_node(key);
                if (comp(root->value.first, key.first)) // key.first > root->value.first
                {
                    this->parent = root; // ?????? ??????
                    root->right = insert(root->right, key);
                } 
                else if (comp(key.first, root->value.first)) // key.first < root->value.first
                {
                    this->parent = root; // ?????? ??????
                    root->left = insert(root->left, key);
                }   
                else
                {
                    this->exists_node = root; // ?????? ??????????????? ?????? ???????????? ?????? ??????
                    return root;
                }
                // ?????? ?????? ??????
                root->height = 1 + max_height(root->left, root->right);

                // ?????? ?????? ??????  
                root = insert_rebalance(root, key);
                
                return root;
            }

            // ????????? ?????? ????????? ???????????? ?????? (remove)
            node_pointer remove_rebalance(node_pointer node)
            {
                int bFactor = getBalanceFactor(node);

                // LL Case OR LR Case
                if (bFactor > 1)
                {
                    if (getBalanceFactor(node->left) < 0)
                        node->left = leftRotate(node->left);
                    return rightRotate(node);
                }
                // RR Case OR RL Case
                if (bFactor < -1)
                {
                    if (getBalanceFactor(node->right) > 0)
                        node->right = rightRotate(node->right);
                    return leftRotate(node);
                }

                return node;
            }

            // ?????? ?????? ????????? ?????? ?????? ?????? ??????
            node_pointer min_node(node_pointer node)
            {
                if (node == NULL)
                    return node;
                if (node->left != NULL)
                    return min_node(node->left);
                else
                    return node;
            }

             // ?????? ?????? ????????? ?????? ?????? ?????? ?????? (const)
            node_pointer min_node(node_pointer node) const
            {
                if (node == NULL)
                    return node;
                if (node->left != NULL)
                    return min_node(node->left);
                else
                    return node;
            }

            // ?????? ?????? ????????? ?????? ??? ?????? ??????
            node_pointer max_node(node_pointer node)
            {
                if (node == NULL)
                    return node;
                if (node->right != NULL)
                    return max_node(node->right);
                else
                    return node;
            }

            // ?????? ?????? ????????? ?????? ??? ?????? ?????? (const)
            node_pointer max_node(node_pointer node) const
            {
                if (node == NULL)
                    return node;
                if (node->right != NULL)
                    return max_node(node->right);
                else
                    return node;
            }

            // ?????? ??????
            node_pointer remove(node_pointer root, typename value_type::first_type key) // ?????? -> value_type::first_type key
            {
                if (root == NULL)
                    return root;
                if (comp(root->value.first, key)) // key > root->value.first
                {
                    this->parent = root;
                    root->right = remove(root->right, key);
                }
                else if (comp(key, root->value.first)) // key < root->value.first
                {
                    this->parent = root;
                    root->left = remove(root->left, key);
                }
                else
                {
                    //??????????????? ????????????
                    if (root->left == NULL && root->right == NULL)
                    {
                        //?????? ????????? root??? ??????
                        if (root->parent == this->super_root)
                        {
                            (this->super_root)->left = NULL;
                            // ?????? insert??? ?????? ??????
                            this->parent = this->super_root;
                        }
                        else
                        {
                            this->parent = NULL; // ?????? ?????? ??????
                        }
                        alloc.destroy(root);
                        alloc.deallocate(root, 1);
                        root = NULL;
                        return NULL;
                    }
                    else if (root->left == NULL) // ??????????????? 1?????? ?????? (right) 
                    {
                        node_pointer tmp = root->right;
                        //?????? ????????? root??? ?????? (??? ?????? ????????? ???????????? ????????? ??????????????? ????????? ??????)
                        if (root->parent == this->super_root)
                        {
                            (this->super_root)->left = tmp;
                            tmp->parent = this->super_root;
                        }
                        else
                        {
                            tmp->parent = (this->parent);
                            this->parent = NULL; // ?????? ?????? ??????
                        }
                        alloc.destroy(root);
                        alloc.deallocate(root, 1);
                        root = NULL;
                        return tmp;
                    }
                    else if (root->right == NULL) // ??????????????? 1?????? ?????? (left) 
                    {
                        node_pointer tmp = root->left;
                         //?????? ????????? root??? ?????? (??? ?????? ????????? ???????????? ????????? ??????????????? ????????? ??????)
                        if (root->parent == this->super_root)
                        {
                            (this->super_root)->left = tmp;
                            tmp->parent = this->super_root;
                        } 
                        else
                        {
                            tmp->parent = this->parent;
                            this->parent = NULL; // ?????? ?????? ??????
                        }
                        alloc.destroy(root);
                        alloc.deallocate(root, 1);
                        root = NULL;
                        return tmp;
                    }
                    else // ??????????????? 2?????? ??????
                    {
                        // ?????? ?????? ?????? ??? ?????? ??? ????????? ???(value)?????? ?????? ??? ?????? ????????? ????????? ?????? ?????? ??????
                        value_copy_node(root, max_node(root->left)->value);
                        this->parent = root;
                        root->left = remove(root->left, root->value.first);
                    }
                }

                // ?????? ?????? ??????
                root->height = 1 + max_height(root->left, root->right);

                // ?????? ?????? ??????  
                root = remove_rebalance(root);
                
                return root;
            }

            // value.first??? ???????????? operator= ?????? ???????????? ?????????????????? ????????? ???????????? ?????????.
            // ?????? ?????? ????????? ????????? ??????????????????(???, deallocate ?????? ??????), ????????? ??????????????? ????????? ????????? ??????
            void value_copy_node(node_pointer src, value_type value)
            {
                node_pointer left = src->left;
                node_pointer right = src->right;
                node_pointer parent = src->parent;
                int height = src->height;

                alloc.destroy(src);
                this->alloc.construct(src, Node(value));
                src->left = left;
                src->right = right;
                src->parent = parent;
                src->height = height;
            }

            /*
                ?????? ????????? ?????? ???????????? ?????? ???????????? ?????????
            */

            // ?????? ?????? Current - Left - Right
            void preorder(node_pointer current)
            {
                if (current != NULL) 
                {
                    std::cout << current->value.first << " ";
                    preorder(current->left);
                    preorder(current->right);
                }
            }
        
            // ?????? ?????? Left - Current - Right
            void inorder(node_pointer current)
            {
                if (current != NULL)
                {
                    inorder(current->left);
                    std::cout << current->value.first << " ";
                    inorder(current->right);
                }
            }
        
            // ?????? ?????? Left - Right - Current
            void postorder(node_pointer current)
            {
                if (current != NULL) 
                {
                    postorder(current->left);
                    postorder(current->right);
                    std::cout << current->value.first << " ";
                }
            }

            // ?????? ?????? (????????? ???????????? ?????????, ????????????)
            void display(node_pointer root, node_pointer node, int level) const
            {
                int i;
                // ?????? ????????? ???????????? ?????? ???????????????.
                if(node != NULL)
                {
                    display(root, node->right, level + 1);
                    std::cout << std::endl;
                    // ?????? ?????? ????????? ???????????????.
                    if(node == root)
                    {
                        std::cout << "Root -> ";
                    }
                    // i??? ??????????????? ?????? ????????? ?????? ????????? ????????? ?????????.
                    for(i = 0; i < level && node != root; i++)
                    {
                        std::cout << "        ";
                    }
                    // ????????? ????????? ???????????? ???????????????.
                    std::cout << node->value.first;

                    // ?????? ?????? ??????
                    if (node->parent == this->super_root)
                        std::cout << "(parent : super_root)" ;
                    else if (node->parent != NULL)
                        std::cout << "(parent : " << node->parent->value.first << ")" ;
                    else
                        std::cout << "(parent : NULL)" ;

                    //?????? ?????? ?????? ??????
                    if (node->right == NULL)
                        std::cout << "(right : NULL, " ;
                    else
                        std::cout << "(right : " << node->right->value.first << ", " ;
                    if (node->left == NULL)
                        std::cout << "left : NULL)" ;
                    else
                        std::cout << "left : "  << node->left->value.first << ")" ;

                    // height ??? bf ??????
                    // std::cout << "(h: "<< node->height << ", bf: " << getBalanceFactor(node) << ")";
                    
                    // ?????? ????????? ??????????????????.
                    display(root, node->left, level + 1);
                }
            }

			//private:
				node_pointer insert_node; // ?????? ????????? ?????? ??????
                node_pointer exists_node; // ?????? ??????????????? ?????? ???????????? ?????? ??????
                node_pointer super_root;
                node_pointer parent;
				node_alloc alloc;
                key_compare comp;
	};

};

#endif