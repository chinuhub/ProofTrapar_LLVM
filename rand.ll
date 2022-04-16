; ModuleID = 'rand.c'
source_filename = "rand.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@lock = dso_local global i32 0, align 4
@state = dso_local global i32 0, align 4
@seed = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  br label %6

6:                                                ; preds = %10, %0
  %7 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %8 = extractvalue { i32, i1 } %7, 1
  %9 = xor i1 %8, true
  br i1 %9, label %10, label %11

10:                                               ; preds = %6
  br label %6

11:                                               ; preds = %6
  %12 = load i32, i32* @state, align 4
  switch i32 %12, label %41 [
    i32 0, label %13
    i32 1, label %19
  ]

13:                                               ; preds = %11
  store i32 1, i32* @seed, align 4
  store i32 1, i32* @state, align 4
  store i32 0, i32* @lock, align 4
  br label %14

14:                                               ; preds = %13, %18
  %15 = load i32, i32* @seed, align 4
  %16 = icmp ne i32 %15, 0
  br i1 %16, label %18, label %17

17:                                               ; preds = %14
  call void @__ASSERT()
  br label %18

18:                                               ; preds = %17, %14
  br label %14

19:                                               ; preds = %11
  store i32 0, i32* @lock, align 4
  store i32 10, i32* %2, align 4
  br label %20

20:                                               ; preds = %24, %19
  %21 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %22 = extractvalue { i32, i1 } %21, 1
  %23 = xor i1 %22, true
  br i1 %23, label %24, label %25

24:                                               ; preds = %20
  br label %20

25:                                               ; preds = %20
  %26 = load i32, i32* @seed, align 4
  store i32 %26, i32* %3, align 4
  store i32 5, i32* %4, align 4
  %27 = load i32, i32* %4, align 4
  %28 = load i32, i32* %3, align 4
  %29 = icmp ne i32 %27, %28
  br i1 %29, label %31, label %30

30:                                               ; preds = %25
  call void @__ASSERT()
  br label %31

31:                                               ; preds = %30, %25
  %32 = load i32, i32* %4, align 4
  store i32 %32, i32* @seed, align 4
  store i32 0, i32* @lock, align 4
  %33 = load i32, i32* %4, align 4
  %34 = load i32, i32* %2, align 4
  %35 = srem i32 %33, %34
  store i32 %35, i32* %5, align 4
  %36 = load i32, i32* %5, align 4
  store i32 %36, i32* %1, align 4
  %37 = load i32, i32* %1, align 4
  %38 = icmp sle i32 %37, 10
  br i1 %38, label %40, label %39

39:                                               ; preds = %31
  call void @__ASSERT()
  br label %40

40:                                               ; preds = %39, %31
  br label %41

41:                                               ; preds = %11, %40
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  br label %6

6:                                                ; preds = %10, %0
  %7 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %8 = extractvalue { i32, i1 } %7, 1
  %9 = xor i1 %8, true
  br i1 %9, label %10, label %11

10:                                               ; preds = %6
  br label %6

11:                                               ; preds = %6
  %12 = load i32, i32* @state, align 4
  switch i32 %12, label %41 [
    i32 0, label %13
    i32 1, label %19
  ]

13:                                               ; preds = %11
  store i32 1, i32* @seed, align 4
  store i32 1, i32* @state, align 4
  store i32 0, i32* @lock, align 4
  br label %14

14:                                               ; preds = %13, %18
  %15 = load i32, i32* @seed, align 4
  %16 = icmp ne i32 %15, 0
  br i1 %16, label %18, label %17

17:                                               ; preds = %14
  call void @__ASSERT()
  br label %18

18:                                               ; preds = %17, %14
  br label %14

19:                                               ; preds = %11
  store i32 0, i32* @lock, align 4
  store i32 10, i32* %2, align 4
  br label %20

20:                                               ; preds = %24, %19
  %21 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %22 = extractvalue { i32, i1 } %21, 1
  %23 = xor i1 %22, true
  br i1 %23, label %24, label %25

24:                                               ; preds = %20
  br label %20

25:                                               ; preds = %20
  %26 = load i32, i32* @seed, align 4
  store i32 %26, i32* %3, align 4
  store i32 5, i32* %4, align 4
  %27 = load i32, i32* %4, align 4
  %28 = load i32, i32* %3, align 4
  %29 = icmp ne i32 %27, %28
  br i1 %29, label %31, label %30

30:                                               ; preds = %25
  call void @__ASSERT()
  br label %31

31:                                               ; preds = %30, %25
  %32 = load i32, i32* %4, align 4
  store i32 %32, i32* @seed, align 4
  store i32 0, i32* @lock, align 4
  %33 = load i32, i32* %4, align 4
  %34 = load i32, i32* %2, align 4
  %35 = srem i32 %33, %34
  store i32 %35, i32* %5, align 4
  %36 = load i32, i32* %5, align 4
  store i32 %36, i32* %1, align 4
  %37 = load i32, i32* %1, align 4
  %38 = icmp sle i32 %37, 10
  br i1 %38, label %40, label %39

39:                                               ; preds = %31
  call void @__ASSERT()
  br label %40

40:                                               ; preds = %39, %31
  br label %41

41:                                               ; preds = %11, %40
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
