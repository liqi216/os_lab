/*
 * elevator clook
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

struct clook_data {
	struct list_head queue;
};

static void clook_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

/* Dispatch functionality doesn't need to be modified. We just need to
add code to print necessary information whenever a dispatch occurs. So this
function just does what it did before, removing the front of the queue and
leaving the rest the same, just with print output now. */
static int clook_dispatch(struct request_queue *q, int force)
{
	struct clook_data *nd = q->elevator->elevator_data;
	char rw = ' '; //Char that will tell which direction I/o is

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);

		/* In order to see which direction the request is, we can compare
		the direction with the constants READ or WRITE (I chose WRITE in this
		case. If dir and WRITE are equal, it was a write. Otherwise it was a
		read. */
		if(rq_data_dir(rq) == WRITE) {
			rw = 'W';
		}
		else {
			rw = 'R';
		}
		//The sector number can be found by just using blk_rq_pos with the req.
		printk("- [CLOOK] dsp %c %lu\n", rw, blk_rq_pos(rq));

		return 1;
	}
	return 0;
}

/* Unlike dispatch, this function does need to be modified significantly. It needs
to use a linked list to hold the queue, traverse the queue one node at a time and
check that node's location with the request location until the request is before the
node. Then we insert the request into the queue at that location and print the same
output as before in the same way.*/
static void clook_add_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;
	/* Since we are using the Kernel's linked list implementation, we must
	initialize a variable to be the head. Will be used to iterate after.
	We also need a character for direction, same as in dispatch */
	struct list_head *currentNode = NULL;
	char rw = ' ';

	/* Traverses the queue one node at a time, each time checking the node's sector with
	the request's sector. As soon as the sector's location is less than the node's we know
	that that is the position it must be at in the queue for the disk head to be optimized
	in its left to right movement. */
	list_for_each(currentNode, &nd->queue) {
		struct request *currentReq = list_entry(currentNode, struct request, queuelist);

		if(rq_end_sector(currentReq) > rq_end_sector(rq)) {
			break;
		}
	}
	list_add_tail(&rq->queuelist, currentNode);

	/* Same printing method as described in the dispatch function */
	if(rq_data_dir(rq) == WRITE) {
		rw = 'W';
	}
	else {
		rw = 'R';
	}
	printk("- [CLOOK] add %c %lu\n", rw, blk_rq_pos(rq));
}

static struct request *
clook_former_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
clook_latter_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

/* I didn't need to implement any changes for the init function for my scheduler
to work properly. This function already initiates the linked list head, so it works
out. */
static int clook_init_queue(struct request_queue *q)
{
	struct clook_data *nd;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd)
		return -ENOMEM;

	INIT_LIST_HEAD(&nd->queue);
	q->elevator->elevator_data = nd;
	return 0;
}

static void clook_exit_queue(struct elevator_queue *e)
{
	struct clook_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_clook = {
	.ops = {
		.elevator_merge_req_fn		= clook_merged_requests,
		.elevator_dispatch_fn		= clook_dispatch,
		.elevator_add_req_fn		= clook_add_request,
		.elevator_former_req_fn		= clook_former_request,
		.elevator_latter_req_fn		= clook_latter_request,
		.elevator_init_fn		= clook_init_queue,
		.elevator_exit_fn		= clook_exit_queue,
	},
	.elevator_name = "clook",
	.elevator_owner = THIS_MODULE,
};

static int __init clook_init(void)
{
	return elv_register(&elevator_clook);
}

static void __exit clook_exit(void)
{
	elv_unregister(&elevator_clook);
}

module_init(clook_init);
module_exit(clook_exit);


MODULE_AUTHOR("Drew Blumenthal");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CLOOK IO scheduler");
