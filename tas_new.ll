; ModuleID = 'tas_new.c'
source_filename = "tas_new.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@mutex = dso_local global i32 0, align 4
@lock = dso_local global i32 0, align 4
@c = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  br label %3

3:                                                ; preds = %0, %47
  store i32 1, i32* %1, align 4
  br label %4

4:                                                ; preds = %8, %3
  %5 = cmpxchg i32* @mutex, i32 0, i32 1 seq_cst seq_cst
  %6 = extractvalue { i32, i1 } %5, 1
  %7 = xor i1 %6, true
  br i1 %7, label %8, label %9

8:                                                ; preds = %4
  br label %4

9:                                                ; preds = %4
  %10 = load i32, i32* @lock, align 4
  store i32 %10, i32* %2, align 4
  store i32 1, i32* @lock, align 4
  store i32 0, i32* @mutex, align 4
  br label %11

11:                                               ; preds = %28, %9
  %12 = load i32, i32* %2, align 4
  %13 = icmp eq i32 %12, 1
  br i1 %13, label %14, label %30

14:                                               ; preds = %11
  %15 = load i32, i32* %1, align 4
  %16 = mul nsw i32 %15, 2
  %17 = load i32, i32* %1, align 4
  %18 = icmp sgt i32 %16, %17
  br i1 %18, label %19, label %22

19:                                               ; preds = %14
  %20 = load i32, i32* %1, align 4
  %21 = mul nsw i32 %20, 2
  store i32 %21, i32* %1, align 4
  br label %22

22:                                               ; preds = %19, %14
  br label %23

23:                                               ; preds = %27, %22
  %24 = cmpxchg i32* @mutex, i32 0, i32 1 seq_cst seq_cst
  %25 = extractvalue { i32, i1 } %24, 1
  %26 = xor i1 %25, true
  br i1 %26, label %27, label %28

27:                                               ; preds = %23
  br label %23

28:                                               ; preds = %23
  %29 = load i32, i32* @lock, align 4
  store i32 %29, i32* %2, align 4
  store i32 1, i32* @lock, align 4
  store i32 0, i32* @mutex, align 4
  br label %11

30:                                               ; preds = %11
  %31 = load i32, i32* %2, align 4
  %32 = load i32, i32* @lock, align 4
  %33 = icmp ne i32 %31, %32
  br i1 %33, label %35, label %34

34:                                               ; preds = %30
  call void @__ASSERT()
  br label %35

35:                                               ; preds = %34, %30
  %36 = load i32, i32* @c, align 4
  %37 = add nsw i32 %36, 1
  store i32 %37, i32* @c, align 4
  %38 = load i32, i32* @c, align 4
  %39 = icmp eq i32 %38, 1
  br i1 %39, label %41, label %40

40:                                               ; preds = %35
  call void @__ASSERT()
  br label %41

41:                                               ; preds = %40, %35
  %42 = load i32, i32* @c, align 4
  %43 = add nsw i32 %42, -1
  store i32 %43, i32* @c, align 4
  %44 = load i32, i32* @lock, align 4
  %45 = icmp ne i32 %44, 0
  br i1 %45, label %47, label %46

46:                                               ; preds = %41
  call void @__ASSERT()
  br label %47

47:                                               ; preds = %46, %41
  store i32 0, i32* @lock, align 4
  br label %3
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
