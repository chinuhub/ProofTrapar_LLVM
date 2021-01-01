; ModuleID = 'peterson.c'
source_filename = "peterson.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@flag1 = dso_local global i32 0, align 4
@flag2 = dso_local global i32 0, align 4
@turn = common dso_local global i32 0, align 4
@x = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  store i32 1, i32* @flag1, align 4
  store i32 1, i32* @turn, align 4
  br label %1

1:                                                ; preds = %9, %0
  %2 = load i32, i32* @flag2, align 4
  %3 = icmp eq i32 %2, 1
  br i1 %3, label %4, label %7

4:                                                ; preds = %1
  %5 = load i32, i32* @turn, align 4
  %6 = icmp eq i32 %5, 1
  br label %7

7:                                                ; preds = %4, %1
  %8 = phi i1 [ false, %1 ], [ %6, %4 ]
  br i1 %8, label %9, label %10

9:                                                ; preds = %7
  br label %1

10:                                               ; preds = %7
  store i32 0, i32* @x, align 4
  %11 = load i32, i32* @x, align 4
  %12 = icmp sle i32 %11, 0
  br i1 %12, label %14, label %13

13:                                               ; preds = %10
  call void @__ASSERT()
  br label %14

14:                                               ; preds = %13, %10
  store i32 0, i32* @flag1, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  store i32 1, i32* @flag2, align 4
  store i32 0, i32* @turn, align 4
  br label %1

1:                                                ; preds = %9, %0
  %2 = load i32, i32* @flag1, align 4
  %3 = icmp eq i32 %2, 1
  br i1 %3, label %4, label %7

4:                                                ; preds = %1
  %5 = load i32, i32* @turn, align 4
  %6 = icmp eq i32 %5, 0
  br label %7

7:                                                ; preds = %4, %1
  %8 = phi i1 [ false, %1 ], [ %6, %4 ]
  br i1 %8, label %9, label %10

9:                                                ; preds = %7
  br label %1

10:                                               ; preds = %7
  store i32 1, i32* @x, align 4
  %11 = load i32, i32* @x, align 4
  %12 = icmp sge i32 %11, 1
  br i1 %12, label %14, label %13

13:                                               ; preds = %10
  call void @__ASSERT()
  br label %14

14:                                               ; preds = %13, %10
  store i32 0, i32* @flag2, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
