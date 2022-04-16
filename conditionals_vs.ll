; ModuleID = 'conditionals_vs.c'
source_filename = "conditionals_vs.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@m = dso_local global i32 0, align 4
@z = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 2, i32* %1, align 4
  store i32 3, i32* %2, align 4
  br label %3

3:                                                ; preds = %7, %0
  %4 = cmpxchg i32* @m, i32 0, i32 1 seq_cst seq_cst
  %5 = extractvalue { i32, i1 } %4, 1
  %6 = xor i1 %5, true
  br i1 %6, label %7, label %8

7:                                                ; preds = %3
  br label %3

8:                                                ; preds = %3
  %9 = load i32, i32* %1, align 4
  %10 = load i32, i32* %2, align 4
  %11 = icmp eq i32 %9, %10
  br i1 %11, label %12, label %13

12:                                               ; preds = %8
  store i32 0, i32* @z, align 4
  br label %14

13:                                               ; preds = %8
  store i32 1, i32* @z, align 4
  br label %14

14:                                               ; preds = %13, %12
  %15 = load i32, i32* @z, align 4
  %16 = icmp eq i32 %15, 0
  br i1 %16, label %17, label %23

17:                                               ; preds = %14
  %18 = load i32, i32* %1, align 4
  %19 = load i32, i32* %2, align 4
  %20 = icmp eq i32 %18, %19
  br i1 %20, label %22, label %21

21:                                               ; preds = %17
  call void @__ASSERT()
  br label %22

22:                                               ; preds = %21, %17
  br label %29

23:                                               ; preds = %14
  %24 = load i32, i32* %1, align 4
  %25 = load i32, i32* %2, align 4
  %26 = icmp ne i32 %24, %25
  br i1 %26, label %28, label %27

27:                                               ; preds = %23
  call void @__ASSERT()
  br label %28

28:                                               ; preds = %27, %23
  br label %29

29:                                               ; preds = %28, %22
  store i32 0, i32* @m, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 3, i32* %1, align 4
  store i32 3, i32* %2, align 4
  br label %3

3:                                                ; preds = %7, %0
  %4 = cmpxchg i32* @m, i32 0, i32 1 seq_cst seq_cst
  %5 = extractvalue { i32, i1 } %4, 1
  %6 = xor i1 %5, true
  br i1 %6, label %7, label %8

7:                                                ; preds = %3
  br label %3

8:                                                ; preds = %3
  %9 = load i32, i32* %1, align 4
  %10 = load i32, i32* %2, align 4
  %11 = icmp eq i32 %9, %10
  br i1 %11, label %12, label %13

12:                                               ; preds = %8
  store i32 0, i32* @z, align 4
  br label %14

13:                                               ; preds = %8
  store i32 1, i32* @z, align 4
  br label %14

14:                                               ; preds = %13, %12
  %15 = load i32, i32* @z, align 4
  %16 = icmp eq i32 %15, 0
  br i1 %16, label %17, label %23

17:                                               ; preds = %14
  %18 = load i32, i32* %1, align 4
  %19 = load i32, i32* %2, align 4
  %20 = icmp eq i32 %18, %19
  br i1 %20, label %22, label %21

21:                                               ; preds = %17
  call void @__ASSERT()
  br label %22

22:                                               ; preds = %21, %17
  br label %29

23:                                               ; preds = %14
  %24 = load i32, i32* %1, align 4
  %25 = load i32, i32* %2, align 4
  %26 = icmp ne i32 %24, %25
  br i1 %26, label %28, label %27

27:                                               ; preds = %23
  call void @__ASSERT()
  br label %28

28:                                               ; preds = %27, %23
  br label %29

29:                                               ; preds = %28, %22
  store i32 0, i32* @m, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
