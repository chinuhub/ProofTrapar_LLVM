; ModuleID = 'bakery_bug.c'
source_filename = "bakery_bug.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@t1 = dso_local global i32 0, align 4
@t2 = dso_local global i32 0, align 4
@x = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  br label %2

2:                                                ; preds = %0, %20
  %3 = load i32, i32* @t2, align 4
  %4 = add nsw i32 %3, 1
  store i32 %4, i32* %1, align 4
  %5 = load i32, i32* %1, align 4
  store i32 %5, i32* @t1, align 4
  br label %6

6:                                                ; preds = %15, %2
  %7 = load i32, i32* @t1, align 4
  %8 = load i32, i32* @t2, align 4
  %9 = icmp sge i32 %7, %8
  br i1 %9, label %10, label %13

10:                                               ; preds = %6
  %11 = load i32, i32* @t2, align 4
  %12 = icmp sgt i32 %11, 0
  br label %13

13:                                               ; preds = %10, %6
  %14 = phi i1 [ false, %6 ], [ %12, %10 ]
  br i1 %14, label %15, label %16

15:                                               ; preds = %13
  br label %6

16:                                               ; preds = %13
  store i32 0, i32* @x, align 4
  %17 = load i32, i32* @x, align 4
  %18 = icmp sle i32 %17, 0
  br i1 %18, label %20, label %19

19:                                               ; preds = %16
  call void @__ASSERT()
  br label %20

20:                                               ; preds = %19, %16
  store i32 0, i32* @t1, align 4
  br label %2
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  br label %1

1:                                                ; preds = %0, %18
  %2 = load i32, i32* @t1, align 4
  %3 = add nsw i32 %2, 1
  store i32 %3, i32* @t2, align 4
  br label %4

4:                                                ; preds = %13, %1
  %5 = load i32, i32* @t2, align 4
  %6 = load i32, i32* @t1, align 4
  %7 = icmp sge i32 %5, %6
  br i1 %7, label %8, label %11

8:                                                ; preds = %4
  %9 = load i32, i32* @t1, align 4
  %10 = icmp sgt i32 %9, 0
  br label %11

11:                                               ; preds = %8, %4
  %12 = phi i1 [ false, %4 ], [ %10, %8 ]
  br i1 %12, label %13, label %14

13:                                               ; preds = %11
  br label %4

14:                                               ; preds = %11
  store i32 1, i32* @x, align 4
  %15 = load i32, i32* @x, align 4
  %16 = icmp sge i32 %15, 1
  br i1 %16, label %18, label %17

17:                                               ; preds = %14
  call void @__ASSERT()
  br label %18

18:                                               ; preds = %17, %14
  store i32 0, i32* @t2, align 4
  br label %1
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
