; ModuleID = 'dekker.c'
source_filename = "dekker.c"
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
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 1, i32* @flag1, align 4
  %3 = load i32, i32* @flag2, align 4
  store i32 %3, i32* %1, align 4
  br label %4

4:                                                ; preds = %19, %0
  %5 = load i32, i32* %1, align 4
  %6 = icmp sge i32 %5, 1
  br i1 %6, label %7, label %21

7:                                                ; preds = %4
  %8 = load i32, i32* @turn, align 4
  store i32 %8, i32* %2, align 4
  %9 = load i32, i32* %2, align 4
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %11, label %19

11:                                               ; preds = %7
  store i32 0, i32* @flag1, align 4
  %12 = load i32, i32* @turn, align 4
  store i32 %12, i32* %2, align 4
  br label %13

13:                                               ; preds = %16, %11
  %14 = load i32, i32* %2, align 4
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %16, label %18

16:                                               ; preds = %13
  %17 = load i32, i32* @turn, align 4
  store i32 %17, i32* %2, align 4
  br label %13

18:                                               ; preds = %13
  store i32 1, i32* @flag1, align 4
  br label %19

19:                                               ; preds = %18, %7
  %20 = load i32, i32* @flag2, align 4
  store i32 %20, i32* %1, align 4
  br label %4

21:                                               ; preds = %4
  store i32 0, i32* @x, align 4
  %22 = load i32, i32* @x, align 4
  %23 = icmp sle i32 %22, 0
  br i1 %23, label %25, label %24

24:                                               ; preds = %21
  call void @__ASSERT()
  br label %25

25:                                               ; preds = %24, %21
  store i32 1, i32* @turn, align 4
  store i32 0, i32* @flag1, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 1, i32* @flag2, align 4
  %3 = load i32, i32* @flag1, align 4
  store i32 %3, i32* %1, align 4
  br label %4

4:                                                ; preds = %19, %0
  %5 = load i32, i32* %1, align 4
  %6 = icmp sge i32 %5, 1
  br i1 %6, label %7, label %21

7:                                                ; preds = %4
  %8 = load i32, i32* @turn, align 4
  store i32 %8, i32* %2, align 4
  %9 = load i32, i32* %2, align 4
  %10 = icmp ne i32 %9, 1
  br i1 %10, label %11, label %19

11:                                               ; preds = %7
  store i32 0, i32* @flag2, align 4
  %12 = load i32, i32* @turn, align 4
  store i32 %12, i32* %2, align 4
  br label %13

13:                                               ; preds = %16, %11
  %14 = load i32, i32* %2, align 4
  %15 = icmp ne i32 %14, 1
  br i1 %15, label %16, label %18

16:                                               ; preds = %13
  %17 = load i32, i32* @turn, align 4
  store i32 %17, i32* %2, align 4
  br label %13

18:                                               ; preds = %13
  store i32 1, i32* @flag2, align 4
  br label %19

19:                                               ; preds = %18, %7
  %20 = load i32, i32* @flag1, align 4
  store i32 %20, i32* %1, align 4
  br label %4

21:                                               ; preds = %4
  store i32 1, i32* @x, align 4
  %22 = load i32, i32* @x, align 4
  %23 = icmp sge i32 %22, 1
  br i1 %23, label %25, label %24

24:                                               ; preds = %21
  call void @__ASSERT()
  br label %25

25:                                               ; preds = %24, %21
  store i32 1, i32* @turn, align 4
  store i32 0, i32* @flag2, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
