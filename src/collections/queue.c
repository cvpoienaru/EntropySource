/**
 * Copyright (c) 2016, Codrin-Victor Poienaru <cvpoienaru@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * This software is provided by the copyright holders and contributors "as is"
 * and any express or implied warranties, including, but not limited to, the
 * implied warranties of merchantability and fitness for a particular purpose
 * are disclaimed. In no event shall the copyright holder or contributors be
 * liable for any direct, indirect, incidental, special, exemplary, or
 * consequential damages (including, but not limited to, procurement of
 * substitute goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether in
 * contract, strict liability, or tort (including negligence or otherwise)
 * arising in any way out of the use of this software, even if advised of the
 * possibility of such damage.
 */

#include <collections/queue.h>

#include <stdlib.h>

#include <global/defs.h>
#include <global/free_type.h>

/**
 * Allocates memory for a queue.
 *
 * @return The address of a newly allocated queue if the operation was
 * successfull, NULL otherwise.
 */
struct es_queue* es_alloc_queue(void)
{
	int status = ES_FAILURE;
	struct es_queue *queue = NULL;

	/* Allocate memory for the queue structure. */
	queue = (struct es_queue*)malloc(sizeof(struct es_queue));
	if(!queue)
		goto exit;

	/* Create a new underlying queue. */
	queue->queue = g_queue_new();
	if(!queue->queue)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, free the partially allocated queue. */
	if(status == ES_FAILURE && queue)
		es_free_queue(&queue, NULL);

	return queue;
}

/**
 * Frees the memory used by a queue.
 *
 * @param queue The queue to be freed.
 * @param free_function The function used to free individual queue elements.
 */
void es_free_queue(
	struct es_queue **queue,
	es_free_data_function free_function)
{
	void *element = NULL;

	/* Perform sanity checks. */
	if(!queue || !(*queue))
		return;

	if((*queue)->queue) {
		if(free_function) {
			/* Free all queue elements. */
			while(!g_queue_is_empty((*queue)->queue)) {
				/* Pop the first queue element. */
				element = g_queue_pop_head((*queue)->queue);

				/* If the element is not NULL, free it. */
				if(element)
					free_function(&element);
			}
		}

		/* Destroy the underlying queue. */
		g_queue_free((*queue)->queue);
	}

	/* Free the queue structure. */
	free(*queue);
	*queue = NULL;
}

/**
 * Creates a new queue.
 *
 * @return The address of a newly allocated queue if the operation was
 * successfull, NULL otherwise.
 */
struct es_queue* es_create_queue(void)
{
	/* Allocate memory for the new queue. */
	return es_alloc_queue();
}

/**
 * Destroys a queue.
 *
 * @param queue The queue to be destroyed.
 * @param free_function The function used to free individual queue elements.
 */
void es_destroy_queue(
	struct es_queue **queue,
	es_free_data_function free_function)
{
	/* Free the given queue. */
	es_free_queue(queue, free_function);
}

/**
 * Validates a queue.
 *
 * @param queue The queue to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_queue(struct es_queue *queue)
{
	/* Perform sanity checks. */
	if(!queue)
		return ES_FAILURE;

	/* Perform field validation. */
	if(!queue->queue)
		return ES_FAILURE;

	return ES_SUCCESS;
}

/**
 * Checks if the specified queue is empty or not.
 *
 * @param queue The queue to be checked.
 * @return TRUE if the given queue is empty, FALSE otherwise.
 */
const int es_check_queue_is_empty(struct es_queue *queue)
{
	/* Perform sanity checks. */
	if(!queue)
		return TRUE;

	if(es_validate_queue(queue) != ES_SUCCESS)
		return TRUE;

	/* Check if the specified queue is empty or not */
	return g_queue_is_empty(queue->queue);
}

/**
 * Pushes the given element in the queue.
 *
 * @param queue The queue where the element should be pushed.
 * @param element The element to be pushed into the queue.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_push_queue(struct es_queue *queue, void *element)
{
	/* Perform sanity checks. */
	if(!queue)
		return ES_FAILURE;

	if(es_validate_queue(queue) != ES_SUCCESS)
		return ES_FAILURE;

	/* Push the given element in the queue. */
	g_queue_push_tail(queue->queue, element);

	return ES_SUCCESS;
}

/**
 * Pops the first element from the specified queue.
 *
 * @param queue The queue from which the element should be popped.
 * @return The address of a queue element if the operation was successfull, NULL
 * otherwise.
 */
void* es_pop_queue(struct es_queue *queue)
{
	/* Perform sanity checks. */
	if(!queue)
		return NULL;

	if(es_validate_queue(queue) != ES_SUCCESS)
		return NULL;

	/* Pop the first element from the specified queue. */
	return g_queue_pop_head(queue->queue);
}
