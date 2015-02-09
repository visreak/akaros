@@
typedef qlock_t;
@@
-struct mutex
+qlock_t

@@
expression E;
@@
-mutex_lock(
+qlock(
 E)

@@
expression E;
@@
-mutex_unlock(
+qunlock(
 E)

// the netif_addr_lock is a spinlock in linux, but it seems to protect the list
// of addresses.  That's the 'qlock' (great name) in plan 9
@@
expression DEV;
@@
-netif_addr_lock(DEV)
+qlock(&DEV->qlock)

@@
expression DEV;
@@
-netif_addr_unlock(DEV)
+qunlock(&DEV->qlock)

@@
expression DEV;
@@
-netif_addr_lock_bh(DEV)
+qlock(&DEV->qlock)

@@
expression DEV;
@@
-netif_addr_unlock_bh(DEV)
+qunlock(&DEV->qlock)
