/*
 * Lottery scheduling class for IC 607 assignement
*/
#include "sched.h"
#include <linux/list.h>

void init_mysched_rq(struct mysched_rq *mysched_rq)
{
    INIT_LIST_HEAD(&mysched_rq->queue);
    mysched_rq->nr_running = 0;
    mysched_rq->max_ticket = 0;
    printk(KERN_INFO "***[MYSCHED] Mysched class is online \n");
}

static void update_curr_mysched(struct rq *rq)
{
}

static void enqueue_task_mysched(struct rq *rq, struct task_struct *p, int flags)
{
    list_add_tail(&rq->mysched.queue, &p->mysched.run_list);
    rq->mysched.nr_running++;
    p->mysched.ticket = 100;
    rq->mysched.max_ticket += p->mysched.ticket; 
    printk(KERN_INFO"***[MYSCHED] Enqueue: nr_running = %d pid = %d\n", rq->mysched.nr_running, p->pid);
}

static void dequeue_task_mysched(struct rq *rq, struct task_struct *p, int flags)
{
	if(!list_empty(&rq->mysched.queue)) {
		list_del_init(&p->mysched.run_list);
		rq->mysched.nr_running--;
		printk(KERN_INFO"***[MYSCHED] Decueue: nr_running = %d pid = %d\n", rq->mysched.nr_running, p->pid);
	}
}

static void yield_task_mysched(struct rq *rq)
{
}

static void check_preempt_curr_mysched(struct rq *rq, struct task_struct *p, int flags)
{
    resched_curr(rq);
}

static void put_prev_task_mysched(struct rq *rq, struct task_struct *prev)
{

}

static struct task_struct * 
pick_next_task_mysched(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
    printk(KERN_INFO "***[MYSCHED] pick next task cpu=%d\n", cpu_of(rq));
    struct task_struct *next_p = NULL;
	struct sched_mysched_entity *next_se = NULL;
	struct mysched_rq *mysched_rq = &rq->mysched;

	if(!mysched_rq->nr_running)
		return NULL;

    int total_ticket = 0;
    //int lucky_ticket = (rand() % rq->mysched.max_ticket) + 1;
    int lucky_ticket = 101;

    printk(KERN_INFO "***[MYSCHED] total = [%d]\n", rq->mysched.max_ticket);
    printk(KERN_INFO "***[MYSCHED] winner = [%d]\n", lucky_ticket);

    struct list_head q;
    list_for_each(q, mysched_rq->queue) {
        //next_p = list_entry(p, struct task_struct, mysched.run_list);
        //next_se = container_of(q, struct sched_mysched_entity, run_list);
	    //next_p = container_of(next_se, struct task_struct, mysched);
        next_p  = container_of(q, struct task_struct, mysched.run_list);
        total_ticket += next_p->mysched.ticket;

        printk(KERN_INFO "***[MYSCHED] pid = [%d] ticket = [%d]\n", next_p->pid, next_p->mysched.ticket);
        printk(KERN_INFO "***[MYSCHED] sum = [%d] \n", total_ticket);

        if(total_ticket >= lucky_ticket) {
            break;
        }
    }

    /*
    while(total_ticket < lucky_ticket) {
        next_se = container_of(mysched_rq->queue.next, struct sched_mysched_entity, run_list);
	    next_p = container_of(next_se, struct task_struct, mysched);

        total_ticket += next_p->mysched.ticket;
        printk(KERN_INFO "***[MYSCHED] pid = [%d] ticket = [%d]\n", next_p->pid, next_p->mysched.ticket);
        printk(KERN_INFO "***[MYSCHED] sum = [%d] \n", total_ticket);


    }
    */

    printk(KERN_INFO "***[MYSCHED] [sum >= winner] Next task pid = [%d] \n", next_p->pid);
	
	//printk(KERN_INFO "\t***[MYSCHED] pick_next_task: cpu=%d,prev->pid=%d,next_p->pid=%d,nr_running=%d\n",cpu_of(rq),prev->pid,next_p->pid,mysched_rq->nr_running);
	return next_p;
}

static int select_task_rq_mysched(struct task_struct *p, int cpu, int sd_flag, int flags)
{
}

static void set_curr_task_mysched(struct rq *rq)
{
}

static void task_tick_mysched(struct rq *rq, struct task_struct *p, int queued)
{
    //need to insert
    struct sched_mysched_entity *me = &p->mysched;
    //printk(KERN_INFO "***[OS18] Tick:::P[%u](mysched = %u): pass = %llu \n", p->pid, me->stride, me->pass);
    printk(KERN_INFO "***[OS18] Tick:::P[%u](mysched_ticket = %u) \n", p->pid, me->ticket);
}

static void prio_changed_mysched(struct rq *rq, struct task_struct *p, int oldprio)
{
}

static void switched_to_mysched(struct rq *rq, struct task_struct *p)
{
    resched_curr(rq);
}

const struct sched_class mysched_sched_class = {
    //pointer to next scheduling class in hierarchy
    .next = &fair_sched_class,

    //when task enters runnable state
    //nr_running++
    .enqueue_task = enqueue_task_mysched,
    //when task needs to go to wait state
    //nr_running--
    .dequeue_task = dequeue_task_mysched,
    .yield_task = yield_task_mysched,

    //check if the task that entered runnable state should prevent current running task
    .check_preempt_curr = check_preempt_curr_mysched,

    //choose the next task to run from run queue
    .pick_next_task = pick_next_task_mysched,
    .put_prev_task = put_prev_task_mysched,

    .set_curr_task = set_curr_task_mysched,
    .task_tick = task_tick_mysched,
    .switched_to = switched_to_mysched,

};





