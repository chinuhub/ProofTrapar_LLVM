; ModuleID = 'szymanski.c'
source_filename = "szymanski.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@flag1 = dso_local global i32 0, align 4
@flag2 = dso_local global i32 0, align 4
@x = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  store i32 1, i32* @flag1, align 4
  br label %1

1:                                                ; preds = %4, %0
  %2 = load i32, i32* @flag2, align 4
  %3 = icmp sge i32 %2, 3
  br i1 %3, label %4, label %5

4:                                                ; preds = %1
  br label %1

5:                                                ; preds = %1
  store i32 3, i32* @flag1, align 4
  %6 = load i32, i32* @flag2, align 4
  %7 = icmp eq i32 %6, 1
  br i1 %7, label %8, label %14

8:                                                ; preds = %5
  store i32 2, i32* @flag1, align 4
  br label %9

9:                                                ; preds = %12, %8
  %10 = load i32, i32* @flag2, align 4
  %11 = icmp ne i32 %10, 4
  br i1 %11, label %12, label %13

12:                                               ; preds = %9
  br label %9

13:                                               ; preds = %9
  br label %14

14:                                               ; preds = %13, %5
  store i32 4, i32* @flag1, align 4
  br label %15

15:                                               ; preds = %18, %14
  %16 = load i32, i32* @flag2, align 4
  %17 = icmp sge i32 %16, 2
  br i1 %17, label %18, label %19

18:                                               ; preds = %15
  br label %15

19:                                               ; preds = %15
  store i32 0, i32* @x, align 4
  %20 = load i32, i32* @x, align 4
  %21 = icmp sle i32 %20, 0
  br i1 %21, label %23, label %22

22:                                               ; preds = %19
  call void @__ASSERT()
  br label %23

23:                                               ; preds = %22, %19
  br label %24

24:                                               ; preds = %32, %23
  %25 = load i32, i32* @flag2, align 4
  %26 = icmp sle i32 2, %25
  br i1 %26, label %27, label %30

27:                                               ; preds = %24
  %28 = load i32, i32* @flag2, align 4
  %29 = icmp sle i32 %28, 3
  br label %30

30:                                               ; preds = %27, %24
  %31 = phi i1 [ false, %24 ], [ %29, %27 ]
  br i1 %31, label %32, label %33

32:                                               ; preds = %30
  br label %24

33:                                               ; preds = %30
  store i32 0, i32* @flag1, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  store i32 1, i32* @flag2, align 4
  br label %1

1:                                                ; preds = %4, %0
  %2 = load i32, i32* @flag1, align 4
  %3 = icmp sge i32 %2, 3
  br i1 %3, label %4, label %5

4:                                                ; preds = %1
  br label %1

5:                                                ; preds = %1
  store i32 3, i32* @flag2, align 4
  %6 = load i32, i32* @flag1, align 4
  %7 = icmp eq i32 %6, 1
  br i1 %7, label %8, label %14

8:                                                ; preds = %5
  store i32 2, i32* @flag2, align 4
  br label %9

9:                                                ; preds = %12, %8
  %10 = load i32, i32* @flag1, align 4
  %11 = icmp ne i32 %10, 4
  br i1 %11, label %12, label %13

12:                                               ; preds = %9
  br label %9

13:                                               ; preds = %9
  br label %14

14:                                               ; preds = %13, %5
  store i32 4, i32* @flag2, align 4
  br label %15

15:                                               ; preds = %18, %14
  %16 = load i32, i32* @flag1, align 4
  %17 = icmp sge i32 %16, 2
  br i1 %17, label %18, label %19

18:                                               ; preds = %15
  br label %15

19:                                               ; preds = %15
  store i32 1, i32* @x, align 4
  %20 = load i32, i32* @x, align 4
  %21 = icmp sge i32 %20, 1
  br i1 %21, label %23, label %22

22:                                               ; preds = %19
  call void @__ASSERT()
  br label %23

23:                                               ; preds = %22, %19
  br label %24

24:                                               ; preds = %32, %23
  %25 = load i32, i32* @flag1, align 4
  %26 = icmp sle i32 2, %25
  br i1 %26, label %27, label %30

27:                                               ; preds = %24
  %28 = load i32, i32* @flag1, align 4
  %29 = icmp sle i32 %28, 3
  br label %30

30:                                               ; preds = %27, %24
  %31 = phi i1 [ false, %24 ], [ %29, %27 ]
  br i1 %31, label %32, label %33

32:                                               ; preds = %30
  br label %24

33:                                               ; preds = %30
  store i32 0, i32* @flag2, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
