; ModuleID = 'time_var_mutex2.c'
source_filename = "time_var_mutex2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@m_inode = dso_local global i32 0, align 4
@m_busy = dso_local global i32 0, align 4
@block = dso_local global i32 0, align 4
@busy = dso_local global i32 0, align 4
@inode = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @allocator() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = cmpxchg i32* @m_inode, i32 0, i32 1 seq_cst seq_cst
  %3 = extractvalue { i32, i1 } %2, 1
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  %7 = load i32, i32* @inode, align 4
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %9, label %16

9:                                                ; preds = %6
  br label %10

10:                                               ; preds = %14, %9
  %11 = cmpxchg i32* @m_busy, i32 0, i32 1 seq_cst seq_cst
  %12 = extractvalue { i32, i1 } %11, 1
  %13 = xor i1 %12, true
  br i1 %13, label %14, label %15

14:                                               ; preds = %10
  br label %10

15:                                               ; preds = %10
  store i32 1, i32* @busy, align 4
  store i32 0, i32* @m_busy, align 4
  store i32 1, i32* @inode, align 4
  br label %16

16:                                               ; preds = %15, %6
  store i32 1, i32* @block, align 4
  %17 = load i32, i32* @block, align 4
  %18 = icmp eq i32 %17, 1
  br i1 %18, label %20, label %19

19:                                               ; preds = %16
  call void @__ASSERT()
  br label %20

20:                                               ; preds = %19, %16
  store i32 0, i32* @m_inode, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @deallocator() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = cmpxchg i32* @m_busy, i32 0, i32 1 seq_cst seq_cst
  %3 = extractvalue { i32, i1 } %2, 1
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  %7 = load i32, i32* @busy, align 4
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %9, label %14

9:                                                ; preds = %6
  store i32 0, i32* @block, align 4
  %10 = load i32, i32* @block, align 4
  %11 = icmp eq i32 %10, 0
  br i1 %11, label %13, label %12

12:                                               ; preds = %9
  call void @__ASSERT()
  br label %13

13:                                               ; preds = %12, %9
  br label %14

14:                                               ; preds = %13, %6
  store i32 0, i32* @m_busy, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
