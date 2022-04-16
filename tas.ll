; ModuleID = 'tas.c'
source_filename = "tas.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@lock = dso_local global i32 0, align 4
@c = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @VERIFIER_atomic_TAS(i32* %0, i32* %1) #0 {
  %3 = alloca i32*, align 8
  %4 = alloca i32*, align 8
  store i32* %0, i32** %3, align 8
  store i32* %1, i32** %4, align 8
  %5 = load i32*, i32** %3, align 8
  %6 = load i32, i32* %5, align 4
  %7 = load i32*, i32** %4, align 8
  store i32 %6, i32* %7, align 4
  %8 = load i32*, i32** %3, align 8
  store i32 1, i32* %8, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @acquire_lock() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 1, i32* %1, align 4
  call void @VERIFIER_atomic_TAS(i32* @lock, i32* %2)
  br label %3

3:                                                ; preds = %14, %0
  %4 = load i32, i32* %2, align 4
  %5 = icmp eq i32 %4, 1
  br i1 %5, label %6, label %15

6:                                                ; preds = %3
  %7 = load i32, i32* %1, align 4
  %8 = mul nsw i32 %7, 2
  %9 = load i32, i32* %1, align 4
  %10 = icmp sgt i32 %8, %9
  br i1 %10, label %11, label %14

11:                                               ; preds = %6
  %12 = load i32, i32* %1, align 4
  %13 = mul nsw i32 %12, 2
  store i32 %13, i32* %1, align 4
  br label %14

14:                                               ; preds = %11, %6
  call void @VERIFIER_atomic_TAS(i32* @lock, i32* %2)
  br label %3

15:                                               ; preds = %3
  %16 = load i32, i32* %2, align 4
  %17 = load i32, i32* @lock, align 4
  %18 = icmp ne i32 %16, %17
  br i1 %18, label %20, label %19

19:                                               ; preds = %15
  call void @__ASSERT()
  br label %20

20:                                               ; preds = %19, %15
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @release_lock() #0 {
  %1 = load i32, i32* @lock, align 4
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %4, label %3

3:                                                ; preds = %0
  call void @__ASSERT()
  br label %4

4:                                                ; preds = %3, %0
  store i32 0, i32* @lock, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  br label %1

1:                                                ; preds = %0, %7
  call void @acquire_lock()
  %2 = load i32, i32* @c, align 4
  %3 = add nsw i32 %2, 1
  store i32 %3, i32* @c, align 4
  %4 = load i32, i32* @c, align 4
  %5 = icmp eq i32 %4, 1
  br i1 %5, label %7, label %6

6:                                                ; preds = %1
  call void @__ASSERT()
  br label %7

7:                                                ; preds = %6, %1
  %8 = load i32, i32* @c, align 4
  %9 = add nsw i32 %8, -1
  store i32 %9, i32* @c, align 4
  call void @release_lock()
  br label %1
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  br label %1

1:                                                ; preds = %0, %7
  call void @acquire_lock()
  %2 = load i32, i32* @c, align 4
  %3 = add nsw i32 %2, 1
  store i32 %3, i32* @c, align 4
  %4 = load i32, i32* @c, align 4
  %5 = icmp eq i32 %4, 1
  br i1 %5, label %7, label %6

6:                                                ; preds = %1
  call void @__ASSERT()
  br label %7

7:                                                ; preds = %6, %1
  %8 = load i32, i32* @c, align 4
  %9 = add nsw i32 %8, -1
  store i32 %9, i32* @c, align 4
  call void @release_lock()
  br label %1
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
