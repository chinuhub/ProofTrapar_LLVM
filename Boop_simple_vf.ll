; ModuleID = 'Boop_simple_vf.c'
source_filename = "Boop_simple_vf.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@flag1 = dso_local global i32 0, align 4
@flag2 = dso_local global i32 0, align 4
@release_thr1 = dso_local global i32 0, align 4
@usecount = dso_local global i32 0, align 4
@locked = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = load i32, i32* @release_thr1, align 4
  %3 = icmp ne i32 %2, 0
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  %7 = load i32, i32* @usecount, align 4
  %8 = add nsw i32 %7, 1
  store i32 %8, i32* @usecount, align 4
  %9 = load i32, i32* @locked, align 4
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %11, label %12

11:                                               ; preds = %6
  br label %13

12:                                               ; preds = %6
  store i32 1, i32* @locked, align 4
  store i32 1, i32* @flag1, align 4
  br label %13

13:                                               ; preds = %12, %11
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* @usecount, align 4
  store i32 0, i32* @locked, align 4
  store i32 1, i32* @release_thr1, align 4
  br label %3

3:                                                ; preds = %0, %7
  %4 = load i32, i32* @flag1, align 4
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %3
  br label %8

7:                                                ; preds = %3
  br label %3

8:                                                ; preds = %6
  br label %9

9:                                                ; preds = %18, %8
  %10 = call i32 @__VERIFIER_nondet_int()
  store i32 %10, i32* %1, align 4
  %11 = load i32, i32* %1, align 4
  %12 = icmp eq i32 %11, 0
  br i1 %12, label %13, label %16

13:                                               ; preds = %9
  store i32 1, i32* %2, align 4
  %14 = load i32, i32* @usecount, align 4
  %15 = sub nsw i32 %14, 1
  store i32 %15, i32* @usecount, align 4
  store i32 0, i32* @locked, align 4
  br label %17

16:                                               ; preds = %9
  store i32 0, i32* %2, align 4
  br label %17

17:                                               ; preds = %16, %13
  br label %18

18:                                               ; preds = %17
  %19 = load i32, i32* %2, align 4
  %20 = icmp ne i32 %19, 0
  br i1 %20, label %9, label %21

21:                                               ; preds = %18
  %22 = load i32, i32* @usecount, align 4
  %23 = sub nsw i32 %22, 1
  store i32 %23, i32* @usecount, align 4
  store i32 0, i32* @locked, align 4
  %24 = load i32, i32* @usecount, align 4
  %25 = icmp ne i32 %24, 0
  br i1 %25, label %26, label %27

26:                                               ; preds = %21
  call void @__ASSERT()
  br label %27

27:                                               ; preds = %26, %21
  ret void
}

declare dso_local i32 @__VERIFIER_nondet_int() #1

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
