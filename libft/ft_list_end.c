/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_list_end.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssicard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/03/25 14:14:09 by ssicard           #+#    #+#             */
/*   Updated: 2017/02/16 15:42:14 by ssicard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_list_end(t_list *lst)
{
	if (lst->next == NULL)
		return (lst);
	return (ft_list_end(lst->next));
}