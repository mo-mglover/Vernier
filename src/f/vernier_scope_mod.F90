!-------------------------------------------------------------------------------
! (c) Crown copyright 2026 Met Office. All rights reserved.
! The file LICENCE, distributed with this code, contains details of the terms
! under which the code may be used.
!-------------------------------------------------------------------------------

module vernier_scope_mod
  use vernier_mod, only: vik, vernier_start, vernier_stop
  implicit none
  private

  !-----------------------------------------------------------------------------
  ! Public types 
  !-----------------------------------------------------------------------------

  type, public :: vernier_scope_type
    private
    integer(kind=vik) :: handle = -999
  contains
    final :: vernier_scope_finalize
  end type vernier_scope_type

  !-----------------------------------------------------------------------------
  ! Public subroutines 
  !-----------------------------------------------------------------------------

  public :: vernier_scope_prime

!-----------------------------------------------------------------------------
! Contained functions / subroutines
!-----------------------------------------------------------------------------

contains

  subroutine vernier_scope_prime(this, region_name)
    use omp_lib
    type(vernier_scope_type), intent(inout) :: this
    character(len=*), intent(in)  :: region_name
    call vernier_start(this%handle, region_name)
  end subroutine vernier_scope_prime

  subroutine vernier_scope_finalize(this)
    use omp_lib
    type(vernier_scope_type) :: this
    call vernier_stop(this%handle)
  end subroutine vernier_scope_finalize

end module vernier_scope_mod

