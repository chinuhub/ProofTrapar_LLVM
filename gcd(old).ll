; ModuleID = 'gcd.c'
source_filename = "gcd.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@a_in = dso_local global i32 8, align 4
@b_in = dso_local global i32 6, align 4
@a = dso_local global i32 8, align 4
@b = dso_local global i32 6, align 4
@l = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  br label %1

1:                                                ; preds = %19, %0
  %2 = load i32, i32* @a, align 4
  %3 = load i32, i32* @b, align 4
  %4 = icmp ne i32 %2, %3
  br i1 %4, label %5, label %20

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %7 = cmpxchg i32* @l, i32 0, i32 1 seq_cst seq_cst
  %8 = extractvalue { i32, i1 } %7, 1
  %9 = xor i1 %8, true
  br i1 %9, label %10, label %11

10:                                               ; preds = %6
  br label %6

11:                                               ; preds = %6
  %12 = load i32, i32* @a, align 4
  %13 = load i32, i32* @b, align 4
  %14 = icmp sgt i32 %12, %13
  br i1 %14, label %15, label %19

15:                                               ; preds = %11
  %16 = load i32, i32* @a, align 4
  %17 = load i32, i32* @b, align 4
  %18 = sub nsw i32 %16, %17
  store i32 %18, i32* @a, align 4
  br label %19

19:                                               ; preds = %15, %11
  store i32 0, i32* @l, align 4
  br label %1

20:                                               ; preds = %1
  %21 = load i32, i32* @a_in, align 4
  %22 = load i32, i32* @a, align 4
  %23 = add nsw i32 %21, %22
  %24 = icmp eq i32 %23, 0
  br i1 %24, label %26, label %25

25:                                               ; preds = %20
  call void @__ASSERT()
  br label %26

26:                                               ; preds = %25, %20
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  br label %1

1:                                                ; preds = %19, %0
  %2 = load i32, i32* @a, align 4
  %3 = load i32, i32* @b, align 4
  %4 = icmp ne i32 %2, %3
  br i1 %4, label %5, label %20

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %7 = cmpxchg i32* @l, i32 0, i32 1 seq_cst seq_cst
  %8 = extractvalue { i32, i1 } %7, 1
  %9 = xor i1 %8, true
  br i1 %9, label %10, label %11

10:                                               ; preds = %6
  br label %6

11:                                               ; preds = %6
  %12 = load i32, i32* @b, align 4
  %13 = load i32, i32* @a, align 4
  %14 = icmp sgt i32 %12, %13
  br i1 %14, label %15, label %19

15:                                               ; preds = %11
  %16 = load i32, i32* @b, align 4
  %17 = load i32, i32* @a, align 4
  %18 = sub nsw i32 %16, %17
  store i32 %18, i32* @b, align 4
  br label %19

19:                                               ; preds = %15, %11
  store i32 0, i32* @l, align 4
  br label %1

20:                                               ; preds = %1
  %21 = load i32, i32* @b_in, align 4
  %22 = load i32, i32* @a, align 4
  %23 = add nsw i32 %21, %22
  %24 = icmp eq i32 %23, 0
  br i1 %24, label %26, label %25

25:                                               ; preds = %20
  call void @__ASSERT()
  br label %26

26:                                               ; preds = %25, %20
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
