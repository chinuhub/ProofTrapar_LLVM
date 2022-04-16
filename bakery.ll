; ModuleID = 'bakery.c'
source_filename = "bakery.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@choosing1 = dso_local global i32 0, align 4
@choosing2 = dso_local global i32 0, align 4
@number1 = dso_local global i32 0, align 4
@number2 = dso_local global i32 0, align 4
@x = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  store i32 1, i32* @choosing1, align 4
  %2 = load i32, i32* @number2, align 4
  %3 = add nsw i32 %2, 1
  store i32 %3, i32* %1, align 4
  %4 = load i32, i32* %1, align 4
  store i32 %4, i32* @number1, align 4
  store i32 0, i32* @choosing1, align 4
  br label %5

5:                                                ; preds = %8, %0
  %6 = load i32, i32* @choosing2, align 4
  %7 = icmp sge i32 %6, 1
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  br label %5

9:                                                ; preds = %5
  br label %10

10:                                               ; preds = %19, %9
  %11 = load i32, i32* @number1, align 4
  %12 = load i32, i32* @number2, align 4
  %13 = icmp sge i32 %11, %12
  br i1 %13, label %14, label %17

14:                                               ; preds = %10
  %15 = load i32, i32* @number2, align 4
  %16 = icmp sgt i32 %15, 0
  br label %17

17:                                               ; preds = %14, %10
  %18 = phi i1 [ false, %10 ], [ %16, %14 ]
  br i1 %18, label %19, label %20

19:                                               ; preds = %17
  br label %10

20:                                               ; preds = %17
  store i32 0, i32* @x, align 4
  %21 = load i32, i32* @x, align 4
  %22 = icmp sle i32 %21, 0
  br i1 %22, label %24, label %23

23:                                               ; preds = %20
  call void @__ASSERT()
  br label %24

24:                                               ; preds = %23, %20
  store i32 0, i32* @number1, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  store i32 1, i32* @choosing2, align 4
  %2 = load i32, i32* @number1, align 4
  %3 = add nsw i32 %2, 1
  store i32 %3, i32* %1, align 4
  %4 = load i32, i32* %1, align 4
  store i32 %4, i32* @number2, align 4
  store i32 0, i32* @choosing2, align 4
  br label %5

5:                                                ; preds = %8, %0
  %6 = load i32, i32* @choosing1, align 4
  %7 = icmp sge i32 %6, 1
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  br label %5

9:                                                ; preds = %5
  br label %10

10:                                               ; preds = %19, %9
  %11 = load i32, i32* @number2, align 4
  %12 = load i32, i32* @number1, align 4
  %13 = icmp sge i32 %11, %12
  br i1 %13, label %14, label %17

14:                                               ; preds = %10
  %15 = load i32, i32* @number1, align 4
  %16 = icmp sgt i32 %15, 0
  br label %17

17:                                               ; preds = %14, %10
  %18 = phi i1 [ false, %10 ], [ %16, %14 ]
  br i1 %18, label %19, label %20

19:                                               ; preds = %17
  br label %10

20:                                               ; preds = %17
  store i32 1, i32* @x, align 4
  %21 = load i32, i32* @x, align 4
  %22 = icmp sge i32 %21, 1
  br i1 %22, label %24, label %23

23:                                               ; preds = %20
  call void @__ASSERT()
  br label %24

24:                                               ; preds = %23, %20
  store i32 0, i32* @number2, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
